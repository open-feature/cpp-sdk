#include "openfeature/hooks/logging_hook.h"

#include <gtest/gtest.h>

#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include "openfeature/evaluation_context.h"
#include "openfeature/exceptions/open_feature_exceptions.h"
#include "openfeature/flag_evaluation_details.h"
#include "openfeature/flag_metadata.h"
#include "openfeature/flag_type_value.h"
#include "openfeature/hook_context.h"
#include "openfeature/hook_data.h"
#include "openfeature/hook_hints.h"
#include "openfeature/hooks/log_level.h"
#include "openfeature/reason.h"
#include "openfeature/value.h"

namespace openfeature {

constexpr std::string_view kClientName = "test-client";
constexpr std::string_view kProviderName = "test-provider";
constexpr std::string_view kTargetingKey = "user-42";
constexpr std::string_view kAttrKey = "tier";
constexpr std::string_view kAttrValue = "gold";
constexpr std::string_view kAttrKeyEmail = "email";
constexpr std::string_view kAttrValueEmail = "alice@example.com";
constexpr std::string_view kAttrKeyAge = "age";
constexpr int64_t kAttrValueAge = 25;
constexpr std::string_view kAttrKeyVersion = "is_beta";
constexpr bool kAttrValueVersion = true;

constexpr std::string_view kBoolFlagKey = "bool-flag";
constexpr bool kDefaultBoolValue = true;

constexpr std::string_view kStringFlagKey = "theme-flag";
constexpr std::string_view kDefaultStringValue = "dark";
constexpr std::string_view kEvaluatedStringValue = "light";
constexpr std::string_view kVariant = "variant-b";

constexpr std::string_view kIntFlagKey = "int-flag";
constexpr int64_t kDefaultIntValue = 100;
constexpr std::string_view kErrorMessage = "flag not found";

struct LogEntry {
  LogLevel level;
  std::string message;
};

class LoggingHookTest : public ::testing::Test {
 protected:
  LoggingHookTest()
      : context_(
            EvaluationContext::Builder()
                .WithTargetingKey(std::string(kTargetingKey))
                .WithAttribute(std::string(kAttrKey), std::string(kAttrValue))
                .Build()),
        client_metadata_{std::string(kClientName)},
        provider_metadata_{std::string(kProviderName)},
        hook_data_(std::make_shared<HookData>()) {}

  LogCallback CreateCapturingLogger(std::vector<LogEntry>& logs) {
    return [&logs](LogLevel level, const std::string& msg) {
      logs.push_back({level, msg});
    };
  }

  EvaluationContext context_;
  Metadata client_metadata_;
  Metadata provider_metadata_;
  std::shared_ptr<HookData> hook_data_;
  HookHints hints_;
};

TEST_F(LoggingHookTest, BeforeStageLogsWithoutContextByDefault) {
  std::vector<LogEntry> logs;
  LoggingHook hook(false, CreateCapturingLogger(logs));

  BoolHookContext ctx(std::string(kBoolFlagKey), FlagValueType::kBoolean,
                      kDefaultBoolValue, context_, client_metadata_,
                      provider_metadata_, hook_data_);

  auto result = hook.Before(ctx, hints_);

  EXPECT_FALSE(result.has_value());
  ASSERT_EQ(logs.size(), 1);
  EXPECT_EQ(logs[0].level, LogLevel::kDebug);
  EXPECT_NE(logs[0].message.find("stage=before"), std::string::npos);
  EXPECT_NE(logs[0].message.find("domain=\"" + std::string(kClientName) + "\""),
            std::string::npos);
  EXPECT_NE(logs[0].message.find("provider_name=\"" +
                                 std::string(kProviderName) + "\""),
            std::string::npos);
  EXPECT_NE(
      logs[0].message.find("flag_key=\"" + std::string(kBoolFlagKey) + "\""),
      std::string::npos);
  EXPECT_NE(logs[0].message.find("default_value=true"), std::string::npos);
  EXPECT_EQ(logs[0].message.find("evaluation_context="), std::string::npos);
}

TEST_F(LoggingHookTest, BeforeStageLogsWithContextWhenOptedIn) {
  std::vector<LogEntry> logs;
  LoggingHook hook(true, CreateCapturingLogger(logs));

  BoolHookContext ctx(std::string(kBoolFlagKey), FlagValueType::kBoolean,
                      kDefaultBoolValue, context_, client_metadata_,
                      provider_metadata_, hook_data_);

  hook.Before(ctx, hints_);

  ASSERT_EQ(logs.size(), 1);
  EXPECT_NE(logs[0].message.find("evaluation_context="), std::string::npos);
  EXPECT_NE(logs[0].message.find(std::string(kTargetingKey)),
            std::string::npos);
  EXPECT_NE(logs[0].message.find(std::string(kAttrValue)), std::string::npos);
}

TEST_F(LoggingHookTest, AfterStageLogsEvaluationOutcome) {
  std::vector<LogEntry> logs;
  LoggingHook hook(false, CreateCapturingLogger(logs));

  StringHookContext ctx(std::string(kStringFlagKey), FlagValueType::kString,
                        std::string(kDefaultStringValue), context_,
                        client_metadata_, provider_metadata_, hook_data_);
  StringFlagEvaluationDetails details(
      std::string(kStringFlagKey), std::string(kEvaluatedStringValue),
      Reason::kTargetingMatch, std::string(kVariant), FlagMetadata());

  hook.After(ctx, details, hints_);

  ASSERT_EQ(logs.size(), 1);
  EXPECT_EQ(logs[0].level, LogLevel::kDebug);
  EXPECT_NE(logs[0].message.find("stage=after"), std::string::npos);
  EXPECT_NE(
      logs[0].message.find("flag_key=\"" + std::string(kStringFlagKey) + "\""),
      std::string::npos);
  EXPECT_NE(logs[0].message.find("default_value=\"" +
                                 std::string(kDefaultStringValue) + "\""),
            std::string::npos);
  EXPECT_NE(logs[0].message.find("reason=\"TARGETING_MATCH\""),
            std::string::npos);
  EXPECT_NE(logs[0].message.find("variant=\"" + std::string(kVariant) + "\""),
            std::string::npos);
  EXPECT_NE(logs[0].message.find("value=\"" +
                                 std::string(kEvaluatedStringValue) + "\""),
            std::string::npos);
}

TEST_F(LoggingHookTest, AfterStageLogsWithContextWhenOptedIn) {
  std::vector<LogEntry> logs;
  LoggingHook hook(true, CreateCapturingLogger(logs));

  StringHookContext ctx(std::string(kStringFlagKey), FlagValueType::kString,
                        std::string(kDefaultStringValue), context_,
                        client_metadata_, provider_metadata_, hook_data_);
  StringFlagEvaluationDetails details(
      std::string(kStringFlagKey), std::string(kEvaluatedStringValue),
      Reason::kTargetingMatch, std::string(kVariant), FlagMetadata());

  hook.After(ctx, details, hints_);

  ASSERT_EQ(logs.size(), 1);
  EXPECT_NE(logs[0].message.find("evaluation_context="), std::string::npos);
  EXPECT_NE(logs[0].message.find(std::string(kTargetingKey)),
            std::string::npos);
  EXPECT_NE(logs[0].message.find(std::string(kAttrValue)), std::string::npos);
}

TEST_F(LoggingHookTest, AfterStageLogsNullVariantWhenNotSet) {
  std::vector<LogEntry> logs;
  LoggingHook hook(false, CreateCapturingLogger(logs));
  StringHookContext ctx(std::string(kStringFlagKey), FlagValueType::kString,
                        std::string(kDefaultStringValue), context_,
                        client_metadata_, provider_metadata_, hook_data_);
  StringFlagEvaluationDetails details(
      std::string(kStringFlagKey), std::string(kEvaluatedStringValue),
      Reason::kDefault, std::nullopt, FlagMetadata());

  hook.After(ctx, details, hints_);

  ASSERT_EQ(logs.size(), 1);
  EXPECT_NE(logs[0].message.find("variant=null"), std::string::npos);
}

TEST_F(LoggingHookTest, ErrorStageLogsExceptionAtErrorLevel) {
  std::vector<LogEntry> logs;
  LoggingHook hook(false, CreateCapturingLogger(logs));

  IntHookContext ctx(std::string(kIntFlagKey), FlagValueType::kInteger,
                     kDefaultIntValue, context_, client_metadata_,
                     provider_metadata_, hook_data_);
  OpenFeatureException error(ErrorCode::kFlagNotFound,
                             std::string(kErrorMessage));

  hook.Error(ctx, error, hints_);

  ASSERT_EQ(logs.size(), 1);
  EXPECT_EQ(logs[0].level, LogLevel::kError);
  EXPECT_NE(logs[0].message.find("stage=error"), std::string::npos);
  EXPECT_NE(logs[0].message.find("error_code=\"FLAG_NOT_FOUND\""),
            std::string::npos);
  EXPECT_NE(logs[0].message.find("error_message=\"" +
                                 std::string(kErrorMessage) + "\""),
            std::string::npos);
}

TEST_F(LoggingHookTest, ErrorStageLogsWithContextWhenOptedIn) {
  std::vector<LogEntry> logs;
  LoggingHook hook(true, CreateCapturingLogger(logs));

  EvaluationContext custom_context =
      EvaluationContext::Builder()
          .WithTargetingKey(std::string(kTargetingKey))
          .WithAttribute(std::string(kAttrKeyEmail),
                         std::string(kAttrValueEmail))
          .WithAttribute(std::string(kAttrKeyAge), kAttrValueAge)
          .WithAttribute(std::string(kAttrKeyVersion), kAttrValueVersion)
          .Build();

  IntHookContext ctx(std::string(kIntFlagKey), FlagValueType::kInteger,
                     kDefaultIntValue, custom_context, client_metadata_,
                     provider_metadata_, hook_data_);
  OpenFeatureException error(ErrorCode::kFlagNotFound,
                             std::string(kErrorMessage));

  hook.Error(ctx, error, hints_);

  ASSERT_EQ(logs.size(), 1);
  EXPECT_NE(logs[0].message.find("evaluation_context="), std::string::npos);
  EXPECT_NE(logs[0].message.find(std::string(kTargetingKey)),
            std::string::npos);
  EXPECT_NE(logs[0].message.find(std::string(kAttrKeyEmail)),
            std::string::npos);
  EXPECT_NE(logs[0].message.find(std::string(kAttrValueEmail)),
            std::string::npos);
  EXPECT_NE(logs[0].message.find(std::string(kAttrKeyAge)), std::string::npos);
  EXPECT_NE(logs[0].message.find(std::to_string(kAttrValueAge)),
            std::string::npos);
  EXPECT_NE(logs[0].message.find(std::string(kAttrKeyVersion)),
            std::string::npos);
  EXPECT_NE(logs[0].message.find(kAttrValueVersion ? "true" : "false"),
            std::string::npos);
}

TEST_F(LoggingHookTest, ErrorStageLogsGeneralErrorForStandardException) {
  std::vector<LogEntry> logs;
  LoggingHook hook(false, CreateCapturingLogger(logs));
  IntHookContext ctx(std::string(kIntFlagKey), FlagValueType::kInteger,
                     kDefaultIntValue, context_, client_metadata_,
                     provider_metadata_, hook_data_);
  std::runtime_error error("unexpected network failure");
  hook.Error(ctx, error, hints_);

  ASSERT_EQ(logs.size(), 1);
  ASSERT_EQ(logs.size(), 1);
  EXPECT_EQ(logs[0].level, LogLevel::kError);
  EXPECT_NE(logs[0].message.find("stage=error"), std::string::npos);
  EXPECT_NE(logs[0].message.find("error_code=\"GENERAL\""), std::string::npos);
  EXPECT_NE(
      logs[0].message.find("error_message=\"unexpected network failure\""),
      std::string::npos);
}

TEST_F(LoggingHookTest, FinallyStageIsNoOp) {
  std::vector<LogEntry> logs;
  LoggingHook hook(true, CreateCapturingLogger(logs));

  BoolHookContext ctx(std::string(kBoolFlagKey), FlagValueType::kBoolean,
                      kDefaultBoolValue, context_, client_metadata_,
                      provider_metadata_, hook_data_);
  BoolFlagEvaluationDetails details(std::string(kBoolFlagKey),
                                    kDefaultBoolValue, Reason::kDefault,
                                    std::nullopt, FlagMetadata());

  hook.Finally(ctx, details, hints_);

  EXPECT_TRUE(logs.empty());
}

TEST_F(LoggingHookTest, DefaultConstructorInitializesSuccessfully) {
  EXPECT_NO_THROW({
    LoggingHook default_hook;
    BoolHookContext ctx(std::string(kBoolFlagKey), FlagValueType::kBoolean,
                        kDefaultBoolValue, context_, client_metadata_,
                        provider_metadata_, hook_data_);
    default_hook.Before(ctx, hints_);
  });
}

}  // namespace openfeature
