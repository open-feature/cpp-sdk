#include "openfeature/hook_support.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <any>
#include <cstdint>
#include <exception>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "mocks/mock_feature_provider.h"
#include "openfeature/client_api.h"
#include "openfeature/error_code.h"
#include "openfeature/evaluation_context.h"
#include "openfeature/evaluation_options.h"
#include "openfeature/flag_evaluation_details.h"
#include "openfeature/flag_metadata.h"
#include "openfeature/general_hook.h"
#include "openfeature/global_context_manager.h"
#include "openfeature/hook.h"
#include "openfeature/hook_context.h"
#include "openfeature/hook_hints.h"
#include "openfeature/hook_manager.h"
#include "openfeature/metadata.h"
#include "openfeature/provider_repository.h"
#include "openfeature/provider_status.h"
#include "openfeature/reason.h"
#include "openfeature/resolution_details.h"

namespace {

using ::openfeature::BoolResolutionDetails;
using ::openfeature::ClientAPI;
using ::openfeature::ErrorCode;
using ::openfeature::EvaluationContext;
using ::openfeature::EvaluationOptions;
using ::openfeature::FlagMetadata;
using ::openfeature::GlobalContextManager;
using ::openfeature::HookManager;
using ::openfeature::HookSupport;
using ::openfeature::MockFeatureProvider;
using ::openfeature::ProviderRepository;
using ::openfeature::ProviderStatus;
using ::openfeature::Reason;
using ::testing::_;
using ::testing::DoAll;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::SaveArg;

class HookSupportTest : public ::testing::Test {
 protected:
  void SetUp() override {
    GlobalContextManager::GetInstance().SetGlobalEvaluationContext(
        EvaluationContext::Builder().Build());
    HookManager::GetInstance().ClearHooks();
  }

  void TearDown() override { HookManager::GetInstance().ClearHooks(); }

  ProviderRepository repo_;
};

class DummyHook1 : public openfeature::BoolHook {};
class DummyHook2 : public openfeature::StringHook {};

// Helper hook that logs lifecycle method calls
class OrderTrackingHook : public openfeature::BoolHook {
 public:
  explicit OrderTrackingHook(std::string name,
                             std::vector<std::string>& execution_log)
      : name_(std::move(name)), execution_log_(execution_log) {}

  std::optional<EvaluationContext> Before(
      const openfeature::HookContext<bool>& /*ctx*/,
      const openfeature::HookHints& /*hints*/) override {
    execution_log_.emplace_back("before:" + name_);
    return std::nullopt;
  }

  void After(const openfeature::HookContext<bool>& /*ctx*/,
             const openfeature::FlagEvaluationDetails<bool>& /*details*/,
             const openfeature::HookHints& /*hints*/) override {
    execution_log_.emplace_back("after:" + name_);
  }

  void Error(const openfeature::HookContext<bool>& /*ctx*/,
             const std::exception& /*exception*/,
             const openfeature::HookHints& /*hints*/) override {
    execution_log_.emplace_back("error:" + name_);
  }

  void Finally(const openfeature::HookContext<bool>& /*ctx*/,
               const openfeature::FlagEvaluationDetails<bool>& /*details*/,
               const openfeature::HookHints& /*hints*/) override {
    execution_log_.emplace_back("finally:" + name_);
  }

 private:
  std::string name_;
  std::vector<std::string>& execution_log_;
};

// Helper hook that mutates context in Before
class ContextMutatingHook : public openfeature::BoolHook {
 public:
  explicit ContextMutatingHook(std::string key, std::string value)
      : key_(std::move(key)), value_(std::move(value)) {}

  std::optional<EvaluationContext> Before(
      const openfeature::HookContext<bool>& /*ctx*/,
      const openfeature::HookHints& /*hints*/) override {
    return EvaluationContext::Builder().WithAttribute(key_, value_).Build();
  }

 private:
  std::string key_;
  std::string value_;
};

// Helper hook to test HookData isolation and persistence
class HookDataTestHook : public openfeature::BoolHook {
 public:
  explicit HookDataTestHook(std::string hook_id)
      : hook_id_(std::move(hook_id)) {}

  std::optional<EvaluationContext> Before(
      const openfeature::HookContext<bool>& ctx,
      const openfeature::HookHints& /*hints*/) override {
    ctx.GetHookData()->Set("id", hook_id_);
    return std::nullopt;
  }

  void After(const openfeature::HookContext<bool>& ctx,
             const openfeature::FlagEvaluationDetails<bool>& /*details*/,
             const openfeature::HookHints& /*hints*/) override {
    const auto* stored_id = ctx.GetHookData()->GetAs<std::string>("id");
    if (stored_id != nullptr) {
      after_id_ = *stored_id;
    }
  }

  void Finally(const openfeature::HookContext<bool>& ctx,
               const openfeature::FlagEvaluationDetails<bool>& /*details*/,
               const openfeature::HookHints& /*hints*/) override {
    const auto* stored_id = ctx.GetHookData()->GetAs<std::string>("id");
    if (stored_id != nullptr) {
      finally_id_ = *stored_id;
    }
  }

  const std::string& GetAfterId() const { return after_id_; }
  const std::string& GetFinallyId() const { return finally_id_; }

 private:
  std::string after_id_;
  std::string finally_id_;
  std::string hook_id_;
};

// Helper hook to test HookHints
class HintsTrackingHook : public openfeature::BoolHook {
 public:
  std::optional<EvaluationContext> Before(
      const openfeature::HookContext<bool>& /*ctx*/,
      const openfeature::HookHints& hints) override {
    auto iterator = hints.find("hint_key");
    if (iterator != hints.end()) {
      const auto* value = std::any_cast<std::string>(&iterator->second);
      if (value != nullptr) {
        before_hint_ = *value;
      }
    }
    return std::nullopt;
  }

  void After(const openfeature::HookContext<bool>& /*ctx*/,
             const openfeature::FlagEvaluationDetails<bool>& /*details*/,
             const openfeature::HookHints& hints) override {
    auto iterator = hints.find("hint_key");
    if (iterator != hints.end()) {
      const auto* value = std::any_cast<std::string>(&iterator->second);
      if (value != nullptr) {
        after_hint_ = *value;
      }
    }
  }

  void Finally(const openfeature::HookContext<bool>& /*ctx*/,
               const openfeature::FlagEvaluationDetails<bool>& /*details*/,
               const openfeature::HookHints& hints) override {
    auto iterator = hints.find("hint_key");
    if (iterator != hints.end()) {
      const auto* value = std::any_cast<std::string>(&iterator->second);
      if (value != nullptr) {
        finally_hint_ = *value;
      }
    }
  }

  const std::string& GetBeforeHint() const { return before_hint_; }
  const std::string& GetAfterHint() const { return after_hint_; }
  const std::string& GetFinallyHint() const { return finally_hint_; }

 private:
  std::string before_hint_;
  std::string after_hint_;
  std::string finally_hint_;
};

enum class ThrowStage : uint8_t { kBefore, kAfter, kError, kFinally };

class ThrowingHook : public openfeature::BoolHook {
 public:
  explicit ThrowingHook(ThrowStage stage, std::string message,
                        std::vector<std::string>& execution_log)
      : stage_(stage),
        message_(std::move(message)),
        execution_log_(execution_log) {}

  std::optional<EvaluationContext> Before(
      const openfeature::HookContext<bool>& /*ctx*/,
      const openfeature::HookHints& /*hints*/) override {
    execution_log_.emplace_back("before");
    if (stage_ == ThrowStage::kBefore) {
      throw std::runtime_error(message_);
    }
    return std::nullopt;
  }

  void After(const openfeature::HookContext<bool>& /*ctx*/,
             const openfeature::FlagEvaluationDetails<bool>& /*details*/,
             const openfeature::HookHints& /*hints*/) override {
    execution_log_.emplace_back("after");
    if (stage_ == ThrowStage::kAfter) {
      throw std::runtime_error(message_);
    }
  }

  void Error(const openfeature::HookContext<bool>& /*ctx*/,
             const std::exception& /*exception*/,
             const openfeature::HookHints& /*hints*/) override {
    execution_log_.emplace_back("error");
    if (stage_ == ThrowStage::kError) {
      throw std::runtime_error(message_);
    }
  }

  void Finally(const openfeature::HookContext<bool>& /*ctx*/,
               const openfeature::FlagEvaluationDetails<bool>& /*details*/,
               const openfeature::HookHints& /*hints*/) override {
    execution_log_.emplace_back("finally");
    if (stage_ == ThrowStage::kFinally) {
      throw std::runtime_error(message_);
    }
  }

 private:
  ThrowStage stage_;
  std::string message_;
  std::vector<std::string>& execution_log_;
};

class StringTrackingHook : public openfeature::StringHook {
 public:
  explicit StringTrackingHook(bool& called) : called_(called) {}
  std::optional<EvaluationContext> Before(
      const openfeature::HookContext<std::string>& /*ctx*/,
      const openfeature::HookHints& /*hints*/) override {
    called_ = true;
    return std::nullopt;
  }

 private:
  bool& called_;
};

class BoolTrackingHook : public openfeature::BoolHook {
 public:
  explicit BoolTrackingHook(bool& called) : called_(called) {}
  std::optional<EvaluationContext> Before(
      const openfeature::HookContext<bool>& /*ctx*/,
      const openfeature::HookHints& /*hints*/) override {
    called_ = true;
    return std::nullopt;
  }

 private:
  bool& called_;
};

}  // namespace

TEST_F(HookSupportTest, InitialStateHasEmptyHooks) {
  ClientAPI client(repo_, "test-domain");
  EXPECT_TRUE(client.GetHooks().empty());
}

TEST_F(HookSupportTest, AddHookAppendsSingleHook) {
  ClientAPI client(repo_, "test-domain");
  std::shared_ptr<openfeature::GeneralHook> hook1 =
      std::make_shared<DummyHook1>();
  client.AddHook(hook1);

  auto hooks = client.GetHooks();
  ASSERT_EQ(hooks.size(), 1);
  EXPECT_EQ(hooks[0], hook1);
}

TEST_F(HookSupportTest, AddHooksAppendsMultipleHooksAndPreservesOrder) {
  ClientAPI client(repo_, "test-domain");
  std::shared_ptr<openfeature::GeneralHook> hook1 =
      std::make_shared<DummyHook1>();
  std::shared_ptr<openfeature::GeneralHook> hook2 =
      std::make_shared<DummyHook2>();

  client.AddHooks({hook1, hook2});

  auto hooks = client.GetHooks();
  ASSERT_EQ(hooks.size(), 2);
  EXPECT_EQ(hooks[0], hook1);
  EXPECT_EQ(hooks[1], hook2);

  std::shared_ptr<openfeature::GeneralHook> hook3 =
      std::make_shared<DummyHook1>();
  client.AddHook(hook3);

  hooks = client.GetHooks();
  ASSERT_EQ(hooks.size(), 3);
  EXPECT_EQ(hooks[0], hook1);
  EXPECT_EQ(hooks[1], hook2);
  EXPECT_EQ(hooks[2], hook3);
}

TEST_F(HookSupportTest, AddHookAndAddHooksFiltersNullptrs) {
  ClientAPI client(repo_, "test-domain");
  client.AddHook(nullptr);
  EXPECT_TRUE(client.GetHooks().empty());

  std::shared_ptr<openfeature::GeneralHook> valid_hook =
      std::make_shared<DummyHook1>();
  client.AddHooks({nullptr, valid_hook, nullptr});

  auto hooks = client.GetHooks();
  ASSERT_EQ(hooks.size(), 1);
  EXPECT_EQ(hooks[0], valid_hook);
}

TEST_F(HookSupportTest, HooksExecuteInCorrectOrderOnSuccess) {
  std::string domain = "order-domain";
  auto mock_provider = std::make_shared<NiceMock<MockFeatureProvider>>();
  ON_CALL(*mock_provider, Init(_)).WillByDefault(Return(absl::OkStatus()));
  ON_CALL(*mock_provider, GetBooleanEvaluation(_, _, _))
      .WillByDefault(testing::Invoke(
          [](std::string_view, bool, const EvaluationContext&)
              -> absl::StatusOr<std::unique_ptr<BoolResolutionDetails>> {
            return std::make_unique<BoolResolutionDetails>(
                true, Reason::kTargetingMatch, std::nullopt, FlagMetadata());
          }));

  std::vector<std::string> execution_log;
  auto api_hook = std::make_shared<OrderTrackingHook>("api", execution_log);
  auto client_hook =
      std::make_shared<OrderTrackingHook>("client", execution_log);
  auto invocation_hook =
      std::make_shared<OrderTrackingHook>("invocation", execution_log);
  auto provider_hook =
      std::make_shared<OrderTrackingHook>("provider", execution_log);

  ON_CALL(*mock_provider, GetHooks())
      .WillByDefault(
          Return(std::vector<std::shared_ptr<openfeature::GeneralHook>>{
              provider_hook}));

  repo_.SetProvider(domain, mock_provider, EvaluationContext::Builder().Build(),
                    true);

  HookManager::GetInstance().AddHook(api_hook);

  ClientAPI client(repo_, domain);
  client.AddHook(client_hook);

  EvaluationOptions options;
  options.hooks = {invocation_hook};

  bool result = client.GetBooleanValue("test_flag", false, options);
  EXPECT_TRUE(result);

  std::vector<std::string> expected_log = {
      "before:api",         "before:client",  "before:invocation",
      "before:provider",    "after:provider", "after:invocation",
      "after:client",       "after:api",      "finally:provider",
      "finally:invocation", "finally:client", "finally:api",
  };
  EXPECT_EQ(execution_log, expected_log);
}

TEST_F(HookSupportTest,
       BeforeHookContextMutationPropagatesToSubsequentHooksAndProvider) {
  std::string domain = "mutation-domain";
  auto mock_provider = std::make_shared<NiceMock<MockFeatureProvider>>();
  ON_CALL(*mock_provider, Init(_)).WillByDefault(Return(absl::OkStatus()));

  EvaluationContext captured_context = EvaluationContext::Builder().Build();
  EXPECT_CALL(*mock_provider, GetBooleanEvaluation(_, _, _))
      .WillOnce(DoAll(
          SaveArg<2>(&captured_context),
          Return(std::make_unique<BoolResolutionDetails>(
              true, Reason::kTargetingMatch, std::nullopt, FlagMetadata()))));

  repo_.SetProvider(domain, mock_provider, EvaluationContext::Builder().Build(),
                    true);

  ClientAPI client(repo_, domain);
  client.AddHook(std::make_shared<ContextMutatingHook>("hook_attr1", "val1"));
  client.AddHook(std::make_shared<ContextMutatingHook>("hook_attr2", "val2"));

  bool result = client.GetBooleanValue("test_flag", false);
  EXPECT_TRUE(result);

  ASSERT_NE(captured_context.GetValue("hook_attr1"), nullptr);
  EXPECT_EQ(
      std::any_cast<std::string>(*captured_context.GetValue("hook_attr1")),
      "val1");

  ASSERT_NE(captured_context.GetValue("hook_attr2"), nullptr);
  EXPECT_EQ(
      std::any_cast<std::string>(*captured_context.GetValue("hook_attr2")),
      "val2");
}

TEST_F(HookSupportTest, HookDataIsIsolatedPerHookAndPersistsAcrossStages) {
  std::string domain = "hook-data-domain";
  auto mock_provider = std::make_shared<NiceMock<MockFeatureProvider>>();
  ON_CALL(*mock_provider, Init(_)).WillByDefault(Return(absl::OkStatus()));
  ON_CALL(*mock_provider, GetBooleanEvaluation(_, _, _))
      .WillByDefault(testing::Invoke(
          [](std::string_view, bool, const EvaluationContext&)
              -> absl::StatusOr<std::unique_ptr<BoolResolutionDetails>> {
            return std::make_unique<BoolResolutionDetails>(
                true, Reason::kTargetingMatch, std::nullopt, FlagMetadata());
          }));

  repo_.SetProvider(domain, mock_provider, EvaluationContext::Builder().Build(),
                    true);

  auto hook_first = std::make_shared<HookDataTestHook>("hook-1");
  auto hook_second = std::make_shared<HookDataTestHook>("hook-2");

  ClientAPI client(repo_, domain);
  client.AddHooks({hook_first, hook_second});

  bool result = client.GetBooleanValue("test_flag", false);
  EXPECT_TRUE(result);

  EXPECT_EQ(hook_first->GetAfterId(), "hook-1");
  EXPECT_EQ(hook_first->GetFinallyId(), "hook-1");

  EXPECT_EQ(hook_second->GetAfterId(), "hook-2");
  EXPECT_EQ(hook_second->GetFinallyId(), "hook-2");
}

TEST_F(HookSupportTest, HookHintsArePropagatedToAllStages) {
  std::string domain = "hints-domain";
  auto mock_provider = std::make_shared<NiceMock<MockFeatureProvider>>();
  ON_CALL(*mock_provider, Init(_)).WillByDefault(Return(absl::OkStatus()));
  ON_CALL(*mock_provider, GetBooleanEvaluation(_, _, _))
      .WillByDefault(testing::Invoke(
          [](std::string_view, bool, const EvaluationContext&)
              -> absl::StatusOr<std::unique_ptr<BoolResolutionDetails>> {
            return std::make_unique<BoolResolutionDetails>(
                true, Reason::kTargetingMatch, std::nullopt, FlagMetadata());
          }));

  repo_.SetProvider(domain, mock_provider, EvaluationContext::Builder().Build(),
                    true);

  auto tracking_hook = std::make_shared<HintsTrackingHook>();
  ClientAPI client(repo_, domain);
  client.AddHook(tracking_hook);

  openfeature::HookHints hints;
  hints["hint_key"] = std::string("test_hint_value");

  EvaluationOptions options;
  options.hook_hints = hints;

  bool result = client.GetBooleanValue("test_flag", false, options);
  EXPECT_TRUE(result);

  EXPECT_EQ(tracking_hook->GetBeforeHint(), "test_hint_value");
  EXPECT_EQ(tracking_hook->GetAfterHint(), "test_hint_value");
  EXPECT_EQ(tracking_hook->GetFinallyHint(), "test_hint_value");
}

TEST_F(HookSupportTest,
       ErrorInBeforeSkipsResolutionAndRunsErrorAndFinallyInReverse) {
  std::string domain = "before-error-domain";
  auto mock_provider = std::make_shared<NiceMock<MockFeatureProvider>>();
  ON_CALL(*mock_provider, Init(_)).WillByDefault(Return(absl::OkStatus()));
  EXPECT_CALL(*mock_provider, GetBooleanEvaluation(_, _, _)).Times(0);

  repo_.SetProvider(domain, mock_provider, EvaluationContext::Builder().Build(),
                    true);

  std::vector<std::string> execution_log;
  auto throwing_hook = std::make_shared<ThrowingHook>(
      ThrowStage::kBefore, "Before hook failed", execution_log);

  ClientAPI client(repo_, domain);
  client.AddHook(throwing_hook);

  auto details = client.GetBooleanDetails("test_flag", false);
  EXPECT_FALSE(details.GetValue());
  EXPECT_EQ(details.GetReason(), Reason::kError);
  EXPECT_EQ(details.GetErrorCode(), ErrorCode::kGeneral);

  std::vector<std::string> expected_log = {"before", "error", "finally"};
  EXPECT_EQ(execution_log, expected_log);
}

TEST_F(HookSupportTest,
       ErrorInAfterMutatesResultAndRunsErrorAndFinallyInReverse) {
  std::string domain = "after-error-domain";
  auto mock_provider = std::make_shared<NiceMock<MockFeatureProvider>>();
  ON_CALL(*mock_provider, Init(_)).WillByDefault(Return(absl::OkStatus()));
  EXPECT_CALL(*mock_provider, GetBooleanEvaluation(_, _, _))
      .WillOnce(Return(std::make_unique<BoolResolutionDetails>(
          true, Reason::kTargetingMatch, std::nullopt, FlagMetadata())));

  repo_.SetProvider(domain, mock_provider, EvaluationContext::Builder().Build(),
                    true);

  std::vector<std::string> execution_log;
  auto throwing_hook = std::make_shared<ThrowingHook>(
      ThrowStage::kAfter, "After hook failed", execution_log);

  ClientAPI client(repo_, domain);
  client.AddHook(throwing_hook);

  auto details = client.GetBooleanDetails("test_flag", false);
  EXPECT_FALSE(details.GetValue());
  EXPECT_EQ(details.GetReason(), Reason::kError);
  EXPECT_EQ(details.GetErrorCode(), ErrorCode::kGeneral);

  std::vector<std::string> expected_log = {"before", "after", "error",
                                           "finally"};
  EXPECT_EQ(execution_log, expected_log);
}

TEST_F(HookSupportTest, ExceptionInErrorOrFinallyDoesNotAbortExecution) {
  std::string domain = "fault-tolerance-domain";
  auto mock_provider = std::make_shared<NiceMock<MockFeatureProvider>>();
  ON_CALL(*mock_provider, Init(_)).WillByDefault(Return(absl::OkStatus()));

  repo_.SetProvider(domain, mock_provider, EvaluationContext::Builder().Build(),
                    true);

  std::vector<std::string> execution_log;
  auto failing_hook = std::make_shared<ThrowingHook>(
      ThrowStage::kError, "Error hook failed", execution_log);
  auto tracking_hook =
      std::make_shared<OrderTrackingHook>("tracker", execution_log);

  // failing_hook throws in Before to trigger Error stage
  auto throwing_before_hook = std::make_shared<ThrowingHook>(
      ThrowStage::kBefore, "Before failed", execution_log);

  ClientAPI client(repo_, domain);
  client.AddHooks({tracking_hook, failing_hook, throwing_before_hook});

  // Client evaluation must not throw
  EXPECT_NO_THROW({
    auto details = client.GetBooleanDetails("test_flag", false);
    EXPECT_FALSE(details.GetValue());
    EXPECT_EQ(details.GetReason(), Reason::kError);
  });
}

TEST_F(HookSupportTest, TypeSpecificHooksExecuteOnlyForMatchingFlagTypes) {
  std::string domain = "type-filtering-domain";
  auto mock_provider = std::make_shared<NiceMock<MockFeatureProvider>>();
  ON_CALL(*mock_provider, Init(_)).WillByDefault(Return(absl::OkStatus()));

  repo_.SetProvider(domain, mock_provider, EvaluationContext::Builder().Build(),
                    true);

  bool string_hook_called = false;
  bool bool_hook_called = false;
  auto string_hook = std::make_shared<StringTrackingHook>(string_hook_called);
  auto bool_hook = std::make_shared<BoolTrackingHook>(bool_hook_called);

  ClientAPI client(repo_, domain);
  client.AddHooks({string_hook, bool_hook});

  client.GetBooleanValue("flag_key", false);
  EXPECT_TRUE(bool_hook_called);
  EXPECT_FALSE(string_hook_called);

  bool_hook_called = false;
  string_hook_called = false;
  client.GetStringValue("string_flag", "default_val");
  EXPECT_TRUE(string_hook_called);
  EXPECT_FALSE(bool_hook_called);
}

TEST_F(HookSupportTest, CollectHooksAggregatesAllTiersInPrecedenceOrder) {
  std::string domain = "collect-hooks-domain";
  auto mock_provider = std::make_shared<NiceMock<MockFeatureProvider>>();

  std::vector<std::string> execution_log;
  auto api_hook = std::make_shared<OrderTrackingHook>("api", execution_log);
  auto client_hook =
      std::make_shared<OrderTrackingHook>("client", execution_log);
  auto invocation_hook =
      std::make_shared<OrderTrackingHook>("invocation", execution_log);
  auto provider_hook =
      std::make_shared<OrderTrackingHook>("provider", execution_log);

  ON_CALL(*mock_provider, GetHooks())
      .WillByDefault(
          Return(std::vector<std::shared_ptr<openfeature::GeneralHook>>{
              provider_hook}));

  HookManager::GetInstance().AddHook(api_hook);

  ClientAPI client(repo_, domain);
  client.AddHook(client_hook);

  EvaluationOptions options;
  options.hooks = {invocation_hook};

  auto collected_hooks =
      HookSupport::CollectHooks(client.GetHooks(), options, mock_provider);
  ASSERT_EQ(collected_hooks.size(), 4);
  EXPECT_EQ(collected_hooks[0], api_hook);
  EXPECT_EQ(collected_hooks[1], client_hook);
  EXPECT_EQ(collected_hooks[2], invocation_hook);
  EXPECT_EQ(collected_hooks[3], provider_hook);
}

TEST_F(HookSupportTest, CollectHooksFiltersNullptrsAcrossAllTiers) {
  std::string domain = "collect-hooks-nulls-domain";
  auto mock_provider = std::make_shared<NiceMock<MockFeatureProvider>>();

  std::vector<std::string> execution_log;
  auto api_hook = std::make_shared<OrderTrackingHook>("api", execution_log);
  auto client_hook =
      std::make_shared<OrderTrackingHook>("client", execution_log);
  auto invocation_hook =
      std::make_shared<OrderTrackingHook>("invocation", execution_log);
  auto provider_hook =
      std::make_shared<OrderTrackingHook>("provider", execution_log);

  HookManager::GetInstance().AddHooks({nullptr, api_hook, nullptr});

  ClientAPI client(repo_, domain);
  client.AddHooks({nullptr, client_hook, nullptr});

  EvaluationOptions options;
  options.hooks = {nullptr, invocation_hook, nullptr};

  ON_CALL(*mock_provider, GetHooks())
      .WillByDefault(
          Return(std::vector<std::shared_ptr<openfeature::GeneralHook>>{
              nullptr, provider_hook, nullptr}));

  auto collected_hooks =
      HookSupport::CollectHooks(client.GetHooks(), options, mock_provider);
  ASSERT_EQ(collected_hooks.size(), 4);
  EXPECT_EQ(collected_hooks[0], api_hook);
  EXPECT_EQ(collected_hooks[1], client_hook);
  EXPECT_EQ(collected_hooks[2], invocation_hook);
  EXPECT_EQ(collected_hooks[3], provider_hook);
}

TEST_F(HookSupportTest, CollectHooksHandlesNulloptOptionsAndNullProvider) {
  std::string domain = "collect-hooks-nullopt-domain";

  std::vector<std::string> execution_log;
  auto api_hook = std::make_shared<OrderTrackingHook>("api", execution_log);
  auto client_hook =
      std::make_shared<OrderTrackingHook>("client", execution_log);

  HookManager::GetInstance().AddHook(api_hook);

  ClientAPI client(repo_, domain);
  client.AddHook(client_hook);

  auto collected_hooks =
      HookSupport::CollectHooks(client.GetHooks(), std::nullopt, nullptr);
  ASSERT_EQ(collected_hooks.size(), 2);
  EXPECT_EQ(collected_hooks[0], api_hook);
  EXPECT_EQ(collected_hooks[1], client_hook);
}

TEST_F(HookSupportTest,
       CreateHookDataMapAllocatesUniqueInstancesAndSharesForDuplicates) {
  std::vector<std::string> execution_log;
  auto first_hook = std::make_shared<OrderTrackingHook>("first", execution_log);
  auto second_hook =
      std::make_shared<OrderTrackingHook>("second", execution_log);

  auto hook_map = HookSupport::CreateHookDataMap(
      {first_hook, second_hook, first_hook, nullptr});
  ASSERT_EQ(hook_map.size(), 2);
  ASSERT_NE(hook_map.find(first_hook.get()), hook_map.end());
  ASSERT_NE(hook_map.find(second_hook.get()), hook_map.end());

  auto first_data = hook_map[first_hook.get()];
  auto second_data = hook_map[second_hook.get()];
  ASSERT_NE(first_data, nullptr);
  ASSERT_NE(second_data, nullptr);
  EXPECT_NE(first_data, second_data);

  first_data->Set("key", std::string("persisted_value"));
  const auto* stored_value =
      hook_map[first_hook.get()]->GetAs<std::string>("key");
  ASSERT_NE(stored_value, nullptr);
  EXPECT_EQ(*stored_value, "persisted_value");
}

TEST_F(HookSupportTest, ProviderNotReadyTriggersErrorAndFinallyHooks) {
  std::string domain = "not-ready-hooks-domain";
  auto mock_provider = std::make_shared<NiceMock<MockFeatureProvider>>();
  EXPECT_CALL(*mock_provider, GetBooleanEvaluation(_, _, _)).Times(0);

  std::vector<std::string> execution_log;
  auto client_hook =
      std::make_shared<OrderTrackingHook>("client", execution_log);
  auto provider_hook =
      std::make_shared<OrderTrackingHook>("provider", execution_log);

  ON_CALL(*mock_provider, GetHooks())
      .WillByDefault(
          Return(std::vector<std::shared_ptr<openfeature::GeneralHook>>{
              provider_hook}));

  repo_.SetProvider(domain, mock_provider, EvaluationContext::Builder().Build(),
                    true);

  auto status_manager = repo_.GetFeatureProviderStatusManager(domain);
  ASSERT_NE(status_manager, nullptr);
  status_manager->SetStatus(ProviderStatus::kNotReady);

  ClientAPI client(repo_, domain);
  client.AddHook(client_hook);

  auto details = client.GetBooleanDetails("test_flag", false);
  EXPECT_FALSE(details.GetValue());
  EXPECT_EQ(details.GetReason(), Reason::kError);
  EXPECT_EQ(details.GetErrorCode(), ErrorCode::kProviderNotReady);

  std::vector<std::string> expected_log = {
      "before:client", "before:provider",  "error:provider",
      "error:client",  "finally:provider", "finally:client"};
  EXPECT_EQ(execution_log, expected_log);
}

TEST_F(HookSupportTest, ProviderFatalTriggersErrorAndFinallyHooks) {
  std::string domain = "fatal-hooks-domain";
  auto mock_provider = std::make_shared<NiceMock<MockFeatureProvider>>();
  EXPECT_CALL(*mock_provider, GetBooleanEvaluation(_, _, _)).Times(0);

  std::vector<std::string> execution_log;
  auto client_hook =
      std::make_shared<OrderTrackingHook>("client", execution_log);
  auto provider_hook =
      std::make_shared<OrderTrackingHook>("provider", execution_log);

  ON_CALL(*mock_provider, GetHooks())
      .WillByDefault(
          Return(std::vector<std::shared_ptr<openfeature::GeneralHook>>{
              provider_hook}));

  repo_.SetProvider(domain, mock_provider, EvaluationContext::Builder().Build(),
                    true);

  auto status_manager = repo_.GetFeatureProviderStatusManager(domain);
  ASSERT_NE(status_manager, nullptr);
  status_manager->SetStatus(ProviderStatus::kFatal);

  ClientAPI client(repo_, domain);
  client.AddHook(client_hook);

  auto details = client.GetBooleanDetails("test_flag", false);
  EXPECT_FALSE(details.GetValue());
  EXPECT_EQ(details.GetReason(), Reason::kError);
  EXPECT_EQ(details.GetErrorCode(), ErrorCode::kProviderFatal);

  std::vector<std::string> expected_log = {
      "before:client", "before:provider",  "error:provider",
      "error:client",  "finally:provider", "finally:client"};
  EXPECT_EQ(execution_log, expected_log);
}
