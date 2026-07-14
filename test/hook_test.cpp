#include "openfeature/hook.h"

#include <gtest/gtest.h>

#include <any>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

#include "openfeature/base_hook.h"
#include "openfeature/evaluation_context.h"
#include "openfeature/flag_evaluation_details.h"
#include "openfeature/flag_metadata.h"
#include "openfeature/hook_context.h"
#include "openfeature/hook_hints.h"
#include "openfeature/value.h"

namespace openfeature {
namespace {

template <typename T>
class TrackingHook : public Hook<T> {
 public:
  TrackingHook() = default;

  std::optional<EvaluationContext> Before(HookContext<T>& ctx,
                                          const HookHints& hints) override {
    before_called = true;
    last_flag_key = ctx.GetFlagKey();
    if (auto it_hints = hints.find("before_hint"); it_hints != hints.end()) {
      last_hint_value = std::any_cast<std::string>(it_hints->second);
    }
    if (return_context.has_value()) {
      return return_context;
    }
    return std::nullopt;
  }

  void After(const HookContext<T>& ctx, const FlagEvaluationDetails<T>& details,
             const HookHints& hints) override {
    after_called = true;
    last_flag_key = ctx.GetFlagKey();
    last_reason = details.GetReason();
    if (auto it_hints = hints.find("after_hint"); it_hints != hints.end()) {
      last_hint_value = std::any_cast<std::string>(it_hints->second);
    }
  }

  void Error(const HookContext<T>& ctx, const std::exception& error,
             const HookHints& hints) override {
    error_called = true;
    last_flag_key = ctx.GetFlagKey();
    last_error_message = error.what();
    if (auto it_hints = hints.find("error_hint"); it_hints != hints.end()) {
      last_hint_value = std::any_cast<std::string>(it_hints->second);
    }
  }

  void Finally(const HookContext<T>& ctx,
               const FlagEvaluationDetails<T>& details,
               const HookHints& hints) override {
    finally_called = true;
    last_flag_key = ctx.GetFlagKey();
    last_reason = details.GetReason();
    if (auto it_hints = hints.find("finally_hint"); it_hints != hints.end()) {
      last_hint_value = std::any_cast<std::string>(it_hints->second);
    }
  }

  void SetReturnContext(std::optional<EvaluationContext> ctx) {
    return_context = std::move(ctx);
  }

  bool before_called = false;
  bool after_called = false;
  bool error_called = false;
  bool finally_called = false;
  std::string last_flag_key;
  std::string last_hint_value;
  std::string last_error_message;
  Reason last_reason = Reason::kUnknown;
  std::optional<EvaluationContext> return_context;
};

class HookTest : public ::testing::Test {
 protected:
  HookTest()
      : initial_ctx_(
            EvaluationContext::Builder().WithTargetingKey("user-123").build()),
        hook_data_(std::make_shared<HookData>()) {}

  EvaluationContext initial_ctx_;
  Metadata client_metadata_{"client-id"};
  Metadata provider_metadata_{"provider-id"};
  std::shared_ptr<HookData> hook_data_;
};

}  // namespace

TEST_F(HookTest, DefaultBeforeReturnsNulloptForAllSpecializations) {
  constexpr bool kBoolValue = true;
  constexpr int kIntValue = 100;
  constexpr double kDoubleValue = 3.14;
  const std::string string_value = "val";
  const Value object_value = Value("obj");
  HookHints hints;

  BoolHook bool_hook;
  BoolHookContext bool_ctx("bool-flag", FlagValueType::kBoolean, kBoolValue,
                           initial_ctx_, client_metadata_, provider_metadata_,
                           hook_data_);
  EXPECT_FALSE(bool_hook.Before(bool_ctx, hints).has_value());

  StringHook string_hook;
  StringHookContext string_ctx("string-flag", FlagValueType::kString,
                               string_value, initial_ctx_, client_metadata_,
                               provider_metadata_, hook_data_);
  EXPECT_FALSE(string_hook.Before(string_ctx, hints).has_value());

  IntHook int_hook;
  IntHookContext int_ctx("int-flag", FlagValueType::kInteger, kIntValue,
                         initial_ctx_, client_metadata_, provider_metadata_,
                         hook_data_);
  EXPECT_FALSE(int_hook.Before(int_ctx, hints).has_value());

  DoubleHook double_hook;
  DoubleHookContext double_ctx("double-flag", FlagValueType::kDouble,
                               kDoubleValue, initial_ctx_, client_metadata_,
                               provider_metadata_, hook_data_);
  EXPECT_FALSE(double_hook.Before(double_ctx, hints).has_value());

  ObjectHook object_hook;
  ObjectHookContext object_ctx("object-flag", FlagValueType::kObject,
                               object_value, initial_ctx_, client_metadata_,
                               provider_metadata_, hook_data_);
  EXPECT_FALSE(object_hook.Before(object_ctx, hints).has_value());
}

TEST_F(HookTest, DefaultAfterErrorAndFinallyAreNoOpsWithoutThrowing) {
  constexpr bool kBoolValue = true;
  BoolHook hook;

  BoolHookContext ctx("bool-flag", FlagValueType::kBoolean, kBoolValue,
                      initial_ctx_, client_metadata_, provider_metadata_,
                      hook_data_);
  BoolFlagEvaluationDetails details("bool-flag", kBoolValue, Reason::kStatic,
                                    std::nullopt, FlagMetadata());
  HookHints hints;
  std::runtime_error error("simulated error");

  EXPECT_NO_THROW(hook.After(ctx, details, hints));
  EXPECT_NO_THROW(hook.Error(ctx, error, hints));
  EXPECT_NO_THROW(hook.Finally(ctx, details, hints));
}

TEST_F(HookTest, OverriddenBeforeCanModifyAndReturnEvaluationContext) {
  constexpr bool kBoolValue = true;
  TrackingHook<bool> hook;
  BoolHookContext ctx("bool-flag", FlagValueType::kBoolean, kBoolValue,
                      initial_ctx_, client_metadata_, provider_metadata_,
                      hook_data_);
  HookHints hints{{"before_hint", std::any(std::string("hint-val"))}};

  EvaluationContext modified_ctx =
      EvaluationContext::Builder()
          .WithTargetingKey("mutated-user")
          .WithAttribute("region", std::string("us-east"))
          .build();
  hook.SetReturnContext(modified_ctx);

  std::optional<EvaluationContext> result = hook.Before(ctx, hints);

  EXPECT_TRUE(hook.before_called);
  EXPECT_EQ(hook.last_flag_key, "bool-flag");
  EXPECT_EQ(hook.last_hint_value, "hint-val");
  ASSERT_TRUE(result.has_value());
  ASSERT_TRUE(result->GetTargetingKey().has_value());
  EXPECT_EQ(result->GetTargetingKey().value(), "mutated-user");
}

TEST_F(HookTest, OverriddenAfterReceivesContextDetailsAndHints) {
  const std::string string_value = "default";
  TrackingHook<std::string> hook;
  StringHookContext ctx("string-flag", FlagValueType::kString, string_value,
                        initial_ctx_, client_metadata_, provider_metadata_,
                        hook_data_);
  StringFlagEvaluationDetails details("string-flag", "variant-val",
                                      Reason::kTargetingMatch, "v1",
                                      FlagMetadata());
  HookHints hints{{"after_hint", std::any(std::string("after-data"))}};

  hook.After(ctx, details, hints);

  EXPECT_TRUE(hook.after_called);
  EXPECT_EQ(hook.last_flag_key, "string-flag");
  EXPECT_EQ(hook.last_reason, Reason::kTargetingMatch);
  EXPECT_EQ(hook.last_hint_value, "after-data");
}

TEST_F(HookTest, OverriddenErrorReceivesExceptionAndHints) {
  constexpr int64_t kIntValue = 42;
  TrackingHook<int64_t> hook;
  IntHookContext ctx("int-flag", FlagValueType::kInteger, kIntValue,
                     initial_ctx_, client_metadata_, provider_metadata_,
                     hook_data_);
  std::runtime_error error("provider timeout error");
  HookHints hints{{"error_hint", std::any(std::string("error-data"))}};

  hook.Error(ctx, error, hints);

  EXPECT_TRUE(hook.error_called);
  EXPECT_EQ(hook.last_flag_key, "int-flag");
  EXPECT_EQ(hook.last_error_message, "provider timeout error");
  EXPECT_EQ(hook.last_hint_value, "error-data");
}

TEST_F(HookTest, OverriddenFinallyReceivesContextDetailsAndHints) {
  constexpr double kDoubleValue = 1.0;
  constexpr double kSecondDoubleValue = 2.718;
  TrackingHook<double> hook;
  DoubleHookContext ctx("double-flag", FlagValueType::kDouble, kDoubleValue,
                        initial_ctx_, client_metadata_, provider_metadata_,
                        hook_data_);
  DoubleFlagEvaluationDetails details("double-flag", kSecondDoubleValue,
                                      Reason::kCached, std::nullopt,
                                      FlagMetadata());
  HookHints hints{{"finally_hint", std::any(std::string("finally-data"))}};

  hook.Finally(ctx, details, hints);

  EXPECT_TRUE(hook.finally_called);
  EXPECT_EQ(hook.last_flag_key, "double-flag");
  EXPECT_EQ(hook.last_reason, Reason::kCached);
  EXPECT_EQ(hook.last_hint_value, "finally-data");
}

TEST_F(HookTest, PolymorphicDestructionViaBaseHookPointer) {
  std::vector<std::unique_ptr<BaseHook>> hooks;
  hooks.push_back(std::make_unique<BoolHook>());
  hooks.push_back(std::make_unique<StringHook>());
  hooks.push_back(std::make_unique<IntHook>());
  hooks.push_back(std::make_unique<DoubleHook>());
  hooks.push_back(std::make_unique<ObjectHook>());

  EXPECT_EQ(hooks.size(), 5);
}

}  // namespace openfeature
