#include "openfeature/client_api.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <cstdint>
#include <future>
#include <memory>
#include <string>
#include <thread>

#include "absl/status/status.h"
#include "mocks/mock_feature_provider.h"
#include "openfeature/evaluation_context.h"
#include "openfeature/evaluation_options.h"
#include "openfeature/global_context_manager.h"
#include "openfeature/global_hook_manager.h"
#include "openfeature/hook.h"
#include "openfeature/hook_support.h"
#include "openfeature/provider_status.h"

using ::openfeature::BoolFlagEvaluationDetails;
using ::openfeature::BoolResolutionDetails;
using ::openfeature::ClientAPI;
using ::openfeature::DoubleFlagEvaluationDetails;
using ::openfeature::ErrorCode;
using ::openfeature::EvaluationContext;
using ::openfeature::EvaluationOptions;
using ::openfeature::FlagMetadata;
using ::openfeature::GlobalContextManager;
using ::openfeature::GlobalHookManager;
using ::openfeature::HookSupport;
using ::openfeature::IntFlagEvaluationDetails;
using ::openfeature::Metadata;
using ::openfeature::MockFeatureProvider;
using ::openfeature::ObjectFlagEvaluationDetails;
using ::openfeature::ProviderRepository;
using ::openfeature::ProviderStatus;
using ::openfeature::Reason;
using ::openfeature::StringFlagEvaluationDetails;
using ::openfeature::Value;
using ::testing::_;
using ::testing::DoAll;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::SaveArg;
using ::testing::StrictMock;

class ClientAPITest : public ::testing::Test {
 protected:
  void SetUp() override {
    // Reset Global Context and Global Hooks to clean states before each test.
    GlobalContextManager::GetInstance().SetGlobalEvaluationContext(
        EvaluationContext::Builder().Build());
    GlobalHookManager::GetInstance().ClearHooks();
  }

  void TearDown() override { GlobalHookManager::GetInstance().ClearHooks(); }

  ProviderRepository repo_;
};

constexpr int kUnknownExceptionError = 43;
constexpr int kSleepTimeMs = 10;
constexpr int64_t kDefaultIntValue = 42;
constexpr double kDefaultDoubleValue = 3.14;
constexpr int kDefaultObjectIntValue = 100;

// Test that the constructor correctly sets the domain in the metadata.
TEST_F(ClientAPITest, ConstructorSetsDomainMetadata) {
  std::string domain = "test-domain";
  ClientAPI client(repo_, domain);

  Metadata metadata = client.GetMetadata();
  EXPECT_EQ(metadata.name, domain);
}

// Test that the provider status is Ready by default.
TEST_F(ClientAPITest, GetProviderStatusDefaultsToReady) {
  ClientAPI client(repo_, "test-domain");
  EXPECT_EQ(client.GetProviderStatus(), ProviderStatus::kReady);
}

// Test setting and getting the EvaluationContext.
TEST_F(ClientAPITest, SetAndGetEvaluationContext) {
  ClientAPI client(repo_, "test-domain");
  EvaluationContext ctx = EvaluationContext::Builder().Build();

  // Verify we can set the context without error.
  EXPECT_NO_THROW(client.SetEvaluationContext(ctx));
}

// Test that GetBooleanValue returns the default value when using the default
// provider.
TEST_F(ClientAPITest, GetBooleanValueReturnsDefaultWithNoopProvider) {
  ClientAPI client(repo_, "test-domain");
  std::string flag_key = "my-boolean-flag";

  EXPECT_TRUE(client.GetBooleanValue(flag_key, true));

  EXPECT_FALSE(client.GetBooleanValue(flag_key, false));
}

// Test that GetStringValue returns the default value when using the default
// provider.
TEST_F(ClientAPITest, GetStringValueReturnsDefaultWithNoopProvider) {
  ClientAPI client(repo_, "test-domain");
  std::string flag_key = "my-string-flag";
  EXPECT_EQ(client.GetStringValue(flag_key, "default"), "default");
}

// Test that GetIntegerValue returns the default value when using the default
// provider.
TEST_F(ClientAPITest, GetIntegerValueReturnsDefaultWithNoopProvider) {
  ClientAPI client(repo_, "test-domain");
  std::string flag_key = "my-integer-flag";
  EXPECT_EQ(client.GetIntegerValue(flag_key, kDefaultIntValue),
            kDefaultIntValue);
}

// Test that GetDoubleValue returns the default value when using the default
// provider.
TEST_F(ClientAPITest, GetDoubleValueReturnsDefaultWithNoopProvider) {
  ClientAPI client(repo_, "test-domain");
  std::string flag_key = "my-double-flag";
  EXPECT_DOUBLE_EQ(client.GetDoubleValue(flag_key, kDefaultDoubleValue),
                   kDefaultDoubleValue);
}

// Test that GetObjectValue returns the default value when using the default
// provider.
TEST_F(ClientAPITest, GetObjectValueReturnsDefaultWithNoopProvider) {
  ClientAPI client(repo_, "test-domain");
  std::string flag_key = "my-object-flag";
  EXPECT_EQ(client.GetObjectValue(flag_key, Value(1)), Value(1));
}

// Test GetBooleanValue with an EvaluationContext passed in.
TEST_F(ClientAPITest, GetBooleanValueWithContextReturnsDefault) {
  ClientAPI client(repo_, "test-domain");
  EvaluationContext ctx = EvaluationContext::Builder().Build();
  std::string flag_key = "my-boolean-flag";

  EXPECT_TRUE(client.GetBooleanValue(flag_key, true, ctx));
  EXPECT_FALSE(client.GetBooleanValue(flag_key, false, ctx));
}

// Test GetStringValue with an EvaluationContext passed in.
TEST_F(ClientAPITest, GetStringValueWithContextReturnsDefault) {
  ClientAPI client(repo_, "test-domain");
  EvaluationContext ctx = EvaluationContext::Builder().Build();
  std::string flag_key = "my-string-flag";
  EXPECT_EQ(client.GetStringValue(flag_key, "default", ctx), "default");
}

// Test that GetIntegerValue with an EvaluationContext passed in.
TEST_F(ClientAPITest, GetIntegerValueWithContextReturnsDefault) {
  ClientAPI client(repo_, "test-domain");
  EvaluationContext ctx = EvaluationContext::Builder().Build();
  std::string flag_key = "my-integer-flag";
  EXPECT_EQ(client.GetIntegerValue(flag_key, kDefaultIntValue, ctx),
            kDefaultIntValue);
}

// Test that GetDoubleValue with an EvaluationContext passed in.
TEST_F(ClientAPITest, GetDoubleValueWithContextReturnsDefault) {
  ClientAPI client(repo_, "test-domain");
  EvaluationContext ctx = EvaluationContext::Builder().Build();
  std::string flag_key = "my-double-flag";
  EXPECT_DOUBLE_EQ(client.GetDoubleValue(flag_key, kDefaultDoubleValue, ctx),
                   kDefaultDoubleValue);
}

// Test that GetObjectValue with an EvaluationContext passed in.
TEST_F(ClientAPITest, GetObjectValueWithContextReturnsDefault) {
  ClientAPI client(repo_, "test-domain");
  EvaluationContext ctx = EvaluationContext::Builder().Build();
  std::string flag_key = "my-object-flag";
  EXPECT_EQ(client.GetObjectValue(flag_key, Value(1), ctx), Value(1));
}

// Test GetBooleanValue with EvaluationOptions.
TEST_F(ClientAPITest, GetBooleanValueWithOptionsReturnsDefault) {
  ClientAPI client(repo_, "test-domain");
  EvaluationOptions options;
  std::string flag_key = "my-boolean-flag";

  EXPECT_TRUE(client.GetBooleanValue(flag_key, true, options));
  EXPECT_FALSE(client.GetBooleanValue(flag_key, false, options));

  EvaluationContext ctx = EvaluationContext::Builder().Build();
  EXPECT_TRUE(client.GetBooleanValue(flag_key, true, ctx, options));
  EXPECT_FALSE(client.GetBooleanValue(flag_key, false, ctx, options));
}

// Test GetStringValue with EvaluationOptions.
TEST_F(ClientAPITest, GetStringValueWithOptionsReturnsDefault) {
  ClientAPI client(repo_, "test-domain");
  EvaluationOptions options;
  std::string flag_key = "my-string-flag";

  EXPECT_EQ(client.GetStringValue(flag_key, "default", options), "default");

  EvaluationContext ctx = EvaluationContext::Builder().Build();
  EXPECT_EQ(client.GetStringValue(flag_key, "default", ctx, options),
            "default");
}

// Test GetIntegerValue with EvaluationOptions.
TEST_F(ClientAPITest, GetIntegerValueWithOptionsReturnsDefault) {
  ClientAPI client(repo_, "test-domain");
  EvaluationOptions options;
  std::string flag_key = "my-integer-flag";

  EXPECT_EQ(client.GetIntegerValue(flag_key, kDefaultIntValue, options),
            kDefaultIntValue);

  EvaluationContext ctx = EvaluationContext::Builder().Build();
  EXPECT_EQ(client.GetIntegerValue(flag_key, kDefaultIntValue, ctx, options),
            kDefaultIntValue);
}

// Test GetDoubleValue with EvaluationOptions.
TEST_F(ClientAPITest, GetDoubleValueWithOptionsReturnsDefault) {
  ClientAPI client(repo_, "test-domain");
  EvaluationOptions options;
  std::string flag_key = "my-double-flag";

  EXPECT_DOUBLE_EQ(
      client.GetDoubleValue(flag_key, kDefaultDoubleValue, options),
      kDefaultDoubleValue);

  EvaluationContext ctx = EvaluationContext::Builder().Build();
  EXPECT_DOUBLE_EQ(
      client.GetDoubleValue(flag_key, kDefaultDoubleValue, ctx, options),
      kDefaultDoubleValue);
}

// Test GetObjectValue with EvaluationOptions.
TEST_F(ClientAPITest, GetObjectValueWithOptionsReturnsDefault) {
  ClientAPI client(repo_, "test-domain");
  EvaluationOptions options;
  std::string flag_key = "my-object-flag";

  EXPECT_EQ(client.GetObjectValue(flag_key, Value(1), options), Value(1));

  EvaluationContext ctx = EvaluationContext::Builder().Build();
  EXPECT_EQ(client.GetObjectValue(flag_key, Value(1), ctx, options), Value(1));
}

// Test GetBooleanDetails returns proper details with NoopProvider.
TEST_F(ClientAPITest, GetBooleanDetailsReturnsDetailsWithNoopProvider) {
  ClientAPI client(repo_, "test-domain");
  std::string flag_key = "my-boolean-flag";
  EvaluationContext ctx = EvaluationContext::Builder().Build();
  EvaluationOptions options;

  auto details1 = client.GetBooleanDetails(flag_key, true);
  EXPECT_EQ(details1.GetFlagKey(), flag_key);
  EXPECT_TRUE(details1.GetValue());
  EXPECT_EQ(details1.GetReason(), Reason::kDefault);
  EXPECT_EQ(details1.GetErrorCode(), std::nullopt);
  EXPECT_EQ(details1.GetErrorMessage(), "");
  EXPECT_EQ(details1.GetVariant(), "default-variant");

  auto details2 = client.GetBooleanDetails(flag_key, false, ctx);
  EXPECT_EQ(details2.GetFlagKey(), flag_key);
  EXPECT_FALSE(details2.GetValue());
  EXPECT_EQ(details2.GetReason(), Reason::kDefault);

  auto details3 = client.GetBooleanDetails(flag_key, true, options);
  EXPECT_EQ(details3.GetFlagKey(), flag_key);
  EXPECT_TRUE(details3.GetValue());

  auto details4 = client.GetBooleanDetails(flag_key, false, ctx, options);
  EXPECT_EQ(details4.GetFlagKey(), flag_key);
  EXPECT_FALSE(details4.GetValue());
}

// Test GetStringDetails returns proper details with NoopProvider.
TEST_F(ClientAPITest, GetStringDetailsReturnsDetailsWithNoopProvider) {
  ClientAPI client(repo_, "test-domain");
  std::string flag_key = "my-string-flag";
  EvaluationContext ctx = EvaluationContext::Builder().Build();
  EvaluationOptions options;

  auto details1 = client.GetStringDetails(flag_key, "default_val");
  EXPECT_EQ(details1.GetFlagKey(), flag_key);
  EXPECT_EQ(details1.GetValue(), "default_val");
  EXPECT_EQ(details1.GetReason(), Reason::kDefault);
  EXPECT_EQ(details1.GetErrorCode(), std::nullopt);

  auto details2 = client.GetStringDetails(flag_key, "default_val", ctx);
  EXPECT_EQ(details2.GetFlagKey(), flag_key);
  EXPECT_EQ(details2.GetValue(), "default_val");

  auto details3 = client.GetStringDetails(flag_key, "default_val", options);
  EXPECT_EQ(details3.GetFlagKey(), flag_key);
  EXPECT_EQ(details3.GetValue(), "default_val");

  auto details4 =
      client.GetStringDetails(flag_key, "default_val", ctx, options);
  EXPECT_EQ(details4.GetFlagKey(), flag_key);
  EXPECT_EQ(details4.GetValue(), "default_val");
}

// Test GetIntegerDetails returns proper details with NoopProvider.
TEST_F(ClientAPITest, GetIntegerDetailsReturnsDetailsWithNoopProvider) {
  ClientAPI client(repo_, "test-domain");
  std::string flag_key = "my-integer-flag";
  EvaluationContext ctx = EvaluationContext::Builder().Build();
  EvaluationOptions options;

  auto details1 = client.GetIntegerDetails(flag_key, kDefaultIntValue);
  EXPECT_EQ(details1.GetFlagKey(), flag_key);
  EXPECT_EQ(details1.GetValue(), kDefaultIntValue);
  EXPECT_EQ(details1.GetReason(), Reason::kDefault);
  EXPECT_EQ(details1.GetErrorCode(), std::nullopt);

  auto details2 = client.GetIntegerDetails(flag_key, kDefaultIntValue, ctx);
  EXPECT_EQ(details2.GetFlagKey(), flag_key);
  EXPECT_EQ(details2.GetValue(), kDefaultIntValue);

  auto details3 = client.GetIntegerDetails(flag_key, kDefaultIntValue, options);
  EXPECT_EQ(details3.GetFlagKey(), flag_key);
  EXPECT_EQ(details3.GetValue(), kDefaultIntValue);

  auto details4 =
      client.GetIntegerDetails(flag_key, kDefaultIntValue, ctx, options);
  EXPECT_EQ(details4.GetFlagKey(), flag_key);
  EXPECT_EQ(details4.GetValue(), kDefaultIntValue);
}

// Test GetDoubleDetails returns proper details with NoopProvider.
TEST_F(ClientAPITest, GetDoubleDetailsReturnsDetailsWithNoopProvider) {
  ClientAPI client(repo_, "test-domain");
  std::string flag_key = "my-double-flag";
  EvaluationContext ctx = EvaluationContext::Builder().Build();
  EvaluationOptions options;

  auto details1 = client.GetDoubleDetails(flag_key, kDefaultDoubleValue);
  EXPECT_EQ(details1.GetFlagKey(), flag_key);
  EXPECT_DOUBLE_EQ(details1.GetValue(), kDefaultDoubleValue);
  EXPECT_EQ(details1.GetReason(), Reason::kDefault);
  EXPECT_EQ(details1.GetErrorCode(), std::nullopt);

  auto details2 = client.GetDoubleDetails(flag_key, kDefaultDoubleValue, ctx);
  EXPECT_EQ(details2.GetFlagKey(), flag_key);
  EXPECT_DOUBLE_EQ(details2.GetValue(), kDefaultDoubleValue);

  auto details3 =
      client.GetDoubleDetails(flag_key, kDefaultDoubleValue, options);
  EXPECT_EQ(details3.GetFlagKey(), flag_key);
  EXPECT_DOUBLE_EQ(details3.GetValue(), kDefaultDoubleValue);

  auto details4 =
      client.GetDoubleDetails(flag_key, kDefaultDoubleValue, ctx, options);
  EXPECT_EQ(details4.GetFlagKey(), flag_key);
  EXPECT_DOUBLE_EQ(details4.GetValue(), kDefaultDoubleValue);
}

// Test GetObjectDetails returns proper details with NoopProvider.
TEST_F(ClientAPITest, GetObjectDetailsReturnsDetailsWithNoopProvider) {
  ClientAPI client(repo_, "test-domain");
  std::string flag_key = "my-object-flag";
  EvaluationContext ctx = EvaluationContext::Builder().Build();
  EvaluationOptions options;

  auto details1 =
      client.GetObjectDetails(flag_key, Value(kDefaultObjectIntValue));
  EXPECT_EQ(details1.GetFlagKey(), flag_key);
  EXPECT_EQ(details1.GetValue(), Value(kDefaultObjectIntValue));
  EXPECT_EQ(details1.GetReason(), Reason::kDefault);
  EXPECT_EQ(details1.GetErrorCode(), std::nullopt);

  auto details2 =
      client.GetObjectDetails(flag_key, Value(kDefaultObjectIntValue), ctx);
  EXPECT_EQ(details2.GetFlagKey(), flag_key);
  EXPECT_EQ(details2.GetValue(), Value(kDefaultObjectIntValue));

  auto details3 =
      client.GetObjectDetails(flag_key, Value(kDefaultObjectIntValue), options);
  EXPECT_EQ(details3.GetFlagKey(), flag_key);
  EXPECT_EQ(details3.GetValue(), Value(kDefaultObjectIntValue));

  auto details4 = client.GetObjectDetails(
      flag_key, Value(kDefaultObjectIntValue), ctx, options);
  EXPECT_EQ(details4.GetFlagKey(), flag_key);
  EXPECT_EQ(details4.GetValue(), Value(kDefaultObjectIntValue));
}

// Test context merging logic indirectly.
TEST_F(ClientAPITest, ContextMergingPrecedence) {
  GlobalContextManager::GetInstance().SetGlobalEvaluationContext(
      EvaluationContext::Builder()
          .WithTargetingKey("global-target")
          .WithAttribute("global_attr", "global_value")
          .WithAttribute("shared_attr_gc", "global_shared_gc")
          .WithAttribute("shared_attr_gci", "global_shared_gci")
          .Build());

  std::shared_ptr<NiceMock<MockFeatureProvider>> mock_provider =
      std::make_shared<NiceMock<MockFeatureProvider>>();

  EXPECT_CALL(*mock_provider, Init(_)).WillOnce(Return(absl::OkStatus()));
  EXPECT_CALL(*mock_provider, Shutdown()).WillOnce(Return(absl::OkStatus()));

  EvaluationContext provider_init_ctx = EvaluationContext::Builder().Build();
  repo_.SetProvider("test-domain", mock_provider, provider_init_ctx, true);

  ClientAPI client(repo_, "test-domain");
  client.SetEvaluationContext(
      EvaluationContext::Builder()
          .WithTargetingKey("client-target")
          .WithAttribute("client_attr", "client_value")
          .WithAttribute("shared_attr_gc", "client_shared_gc")
          .WithAttribute("shared_attr_gci", "client_shared_gci")
          .Build());

  EvaluationContext invocation_ctx =
      EvaluationContext::Builder()
          .WithTargetingKey("invocation-target")
          .WithAttribute("invocation_attr", "invocation_value")
          .WithAttribute("shared_attr_gci", "invocation_shared_gci")
          .Build();

  std::string flag_key = "my-test-flag";
  bool default_value = false;
  bool expected_value = true;

  EvaluationContext captured_merged_ctx = EvaluationContext::Builder().Build();

  // Expect the provider's GetBooleanEvaluation to be called with the merged
  // context.
  EXPECT_CALL(*mock_provider, GetBooleanEvaluation(flag_key, default_value, _))
      .WillOnce(DoAll(SaveArg<2>(&captured_merged_ctx),
                      Return(std::make_unique<BoolResolutionDetails>(
                          expected_value, Reason::kTargetingMatch, std::nullopt,
                          FlagMetadata()))));

  // This call will trigger the context merging and evaluation.
  EXPECT_EQ(client.GetBooleanValue(flag_key, default_value, invocation_ctx),
            expected_value);

  ASSERT_TRUE(captured_merged_ctx.GetTargetingKey().has_value());
  EXPECT_EQ(captured_merged_ctx.GetTargetingKey().value(), "invocation-target");

  ASSERT_NE(captured_merged_ctx.GetValue("global_attr"), nullptr);
  EXPECT_EQ(
      std::any_cast<std::string>(*captured_merged_ctx.GetValue("global_attr")),
      "global_value");

  ASSERT_NE(captured_merged_ctx.GetValue("client_attr"), nullptr);
  EXPECT_EQ(
      std::any_cast<std::string>(*captured_merged_ctx.GetValue("client_attr")),
      "client_value");

  ASSERT_NE(captured_merged_ctx.GetValue("invocation_attr"), nullptr);
  EXPECT_EQ(std::any_cast<std::string>(
                *captured_merged_ctx.GetValue("invocation_attr")),
            "invocation_value");

  ASSERT_NE(captured_merged_ctx.GetValue("shared_attr_gc"), nullptr);
  EXPECT_EQ(std::any_cast<std::string>(
                *captured_merged_ctx.GetValue("shared_attr_gc")),
            "client_shared_gc");

  ASSERT_NE(captured_merged_ctx.GetValue("shared_attr_gci"), nullptr);
  EXPECT_EQ(std::any_cast<std::string>(
                *captured_merged_ctx.GetValue("shared_attr_gci")),
            "invocation_shared_gci");

  EXPECT_EQ(captured_merged_ctx.GetAttributes().size(), 5);
}

// Test behavior when the domain is empty.
TEST_F(ClientAPITest, WorksWithEmptyDomain) {
  ClientAPI client(repo_, "");
  EXPECT_EQ(client.GetMetadata().name, "");
  EXPECT_TRUE(client.GetBooleanValue("flag", true));
}

TEST_F(ClientAPITest, EvaluateFlagHandlesProviderErrorStatus) {
  auto mock_provider = std::make_shared<NiceMock<MockFeatureProvider>>();

  EXPECT_CALL(*mock_provider, GetBooleanEvaluation(_, _, _))
      .Times(2)
      .WillRepeatedly(testing::Invoke(
          [](std::string_view, bool, const EvaluationContext&)
              -> absl::StatusOr<std::unique_ptr<BoolResolutionDetails>> {
            return absl::InternalError("Simulated provider error");
          }));

  repo_.SetProvider("test-domain", mock_provider,
                    EvaluationContext::Builder().Build(), true);
  ClientAPI client(repo_, "test-domain");

  EXPECT_FALSE(client.GetBooleanValue("flag", false));
  EXPECT_TRUE(client.GetBooleanValue("flag", true));
}

TEST_F(ClientAPITest, GetDetailsPropagatesProviderVariantAndMetadata) {
  auto mock_provider = std::make_shared<NiceMock<MockFeatureProvider>>();
  FlagMetadata metadata;
  metadata.data["meta_key"] = std::string("meta_val");

  EXPECT_CALL(*mock_provider, GetBooleanEvaluation("flag", false, _))
      .WillOnce(Return(std::make_unique<BoolResolutionDetails>(
          true, Reason::kTargetingMatch, "variant_a", metadata)));

  repo_.SetProvider("test-domain", mock_provider,
                    EvaluationContext::Builder().Build(), true);
  ClientAPI client(repo_, "test-domain");

  auto details = client.GetBooleanDetails("flag", false);
  EXPECT_EQ(details.GetFlagKey(), "flag");
  EXPECT_TRUE(details.GetValue());
  EXPECT_EQ(details.GetReason(), Reason::kTargetingMatch);
  EXPECT_EQ(details.GetVariant(), "variant_a");
  ASSERT_EQ(details.GetFlagMetadata().data.count("meta_key"), 1);
  EXPECT_EQ(
      std::get<std::string>(details.GetFlagMetadata().data.at("meta_key")),
      "meta_val");
  EXPECT_EQ(details.GetErrorCode(), std::nullopt);
}

TEST_F(ClientAPITest, GetDetailsHandlesProviderErrors) {
  auto mock_provider = std::make_shared<NiceMock<MockFeatureProvider>>();

  EXPECT_CALL(*mock_provider, GetBooleanEvaluation("flag", false, _))
      .WillOnce(Return(absl::InternalError("Provider failed")));

  repo_.SetProvider("test-domain", mock_provider,
                    EvaluationContext::Builder().Build(), true);
  ClientAPI client(repo_, "test-domain");

  auto details = client.GetBooleanDetails("flag", false);
  EXPECT_EQ(details.GetFlagKey(), "flag");
  EXPECT_FALSE(details.GetValue());
  EXPECT_EQ(details.GetReason(), Reason::kError);
  EXPECT_EQ(details.GetErrorCode(), ErrorCode::kGeneral);
  EXPECT_EQ(details.GetErrorMessage(), "Provider failed");
}

TEST_F(ClientAPITest, EvaluateFlagHandlesProviderNullResolutionDetails) {
  auto mock_provider = std::make_shared<NiceMock<MockFeatureProvider>>();

  EXPECT_CALL(*mock_provider, GetBooleanEvaluation(_, _, _))
      .Times(2)
      .WillRepeatedly(testing::Invoke(
          [](std::string_view, bool, const EvaluationContext&)
              -> absl::StatusOr<std::unique_ptr<BoolResolutionDetails>> {
            return std::unique_ptr<BoolResolutionDetails>(nullptr);
          }));

  repo_.SetProvider("test-domain", mock_provider,
                    EvaluationContext::Builder().Build(), true);
  ClientAPI client(repo_, "test-domain");

  EXPECT_FALSE(client.GetBooleanValue("flag", false));
  EXPECT_TRUE(client.GetBooleanValue("flag", true));
}

TEST_F(ClientAPITest, EvaluateFlagHandlesProviderStdException) {
  auto mock_provider = std::make_shared<NiceMock<MockFeatureProvider>>();

  EXPECT_CALL(*mock_provider, GetBooleanEvaluation(_, _, _))
      .Times(2)
      .WillRepeatedly(testing::Invoke(
          [](std::string_view, bool, const EvaluationContext&)
              -> absl::StatusOr<std::unique_ptr<BoolResolutionDetails>> {
            throw std::runtime_error("Simulated standard exception");
            return absl::InternalError("unreachable");
          }));

  repo_.SetProvider("test-domain", mock_provider,
                    EvaluationContext::Builder().Build(), true);
  ClientAPI client(repo_, "test-domain");

  EXPECT_FALSE(client.GetBooleanValue("flag", false));
  EXPECT_TRUE(client.GetBooleanValue("flag", true));
}

TEST_F(ClientAPITest, EvaluateFlagHandlesProviderUnknownException) {
  auto mock_provider = std::make_shared<NiceMock<MockFeatureProvider>>();

  EXPECT_CALL(*mock_provider, GetBooleanEvaluation(_, _, _))
      .Times(2)
      .WillRepeatedly(testing::Invoke(
          [](std::string_view, bool, const EvaluationContext&)
              -> absl::StatusOr<std::unique_ptr<BoolResolutionDetails>> {
            throw kUnknownExceptionError;
            return absl::InternalError("unreachable");
          }));

  repo_.SetProvider("test-domain", mock_provider,
                    EvaluationContext::Builder().Build(), true);
  ClientAPI client(repo_, "test-domain");

  EXPECT_FALSE(client.GetBooleanValue("flag", false));
  EXPECT_TRUE(client.GetBooleanValue("flag", true));
}

TEST_F(ClientAPITest, EvaluateFlagBlocksWhenProviderNotReady) {
  auto mock_provider = std::make_shared<NiceMock<MockFeatureProvider>>();
  EXPECT_CALL(*mock_provider, GetBooleanEvaluation(_, _, _)).Times(0);
  repo_.SetProvider("test-domain", mock_provider,
                    EvaluationContext::Builder().Build(), true);

  auto status_manager = repo_.GetFeatureProviderStatusManager("test-domain");
  ASSERT_NE(status_manager, nullptr);
  status_manager->SetStatus(ProviderStatus::kNotReady);
  ClientAPI client(repo_, "test-domain");

  EXPECT_TRUE(client.GetBooleanValue("flag", true));
  EXPECT_FALSE(client.GetBooleanValue("flag", false));
}

TEST_F(ClientAPITest, EvaluateFlagBlocksWhenProviderFatal) {
  auto mock_provider = std::make_shared<NiceMock<MockFeatureProvider>>();
  EXPECT_CALL(*mock_provider, GetBooleanEvaluation(_, _, _)).Times(0);
  repo_.SetProvider("test-domain", mock_provider,
                    EvaluationContext::Builder().Build(), true);

  auto status_manager = repo_.GetFeatureProviderStatusManager("test-domain");
  ASSERT_NE(status_manager, nullptr);
  status_manager->SetStatus(ProviderStatus::kFatal);
  ClientAPI client(repo_, "test-domain");

  EXPECT_TRUE(client.GetBooleanValue("flag", true));
}

TEST_F(ClientAPITest, EvaluateFlagProceedsWhenProviderInErrorState) {
  auto mock_provider = std::make_shared<NiceMock<MockFeatureProvider>>();
  EXPECT_CALL(*mock_provider, GetBooleanEvaluation("flag", false, _))
      .WillOnce(Return(std::make_unique<BoolResolutionDetails>(
          true, Reason::kCached, std::nullopt, FlagMetadata())));
  repo_.SetProvider("test-domain", mock_provider,
                    EvaluationContext::Builder().Build(), true);

  auto status_manager = repo_.GetFeatureProviderStatusManager("test-domain");
  ASSERT_NE(status_manager, nullptr);
  status_manager->SetStatus(ProviderStatus::kError);
  ClientAPI client(repo_, "test-domain");

  EXPECT_TRUE(client.GetBooleanValue("flag", false));
}

TEST_F(ClientAPITest, EvaluateFlagProceedsWhenProviderInStaleState) {
  auto mock_provider = std::make_shared<NiceMock<MockFeatureProvider>>();
  EXPECT_CALL(*mock_provider, GetBooleanEvaluation("flag", false, _))
      .WillOnce(Return(std::make_unique<BoolResolutionDetails>(
          true, Reason::kCached, std::nullopt, FlagMetadata())));
  repo_.SetProvider("test-domain", mock_provider,
                    EvaluationContext::Builder().Build(), true);

  auto status_manager = repo_.GetFeatureProviderStatusManager("test-domain");
  ASSERT_NE(status_manager, nullptr);
  status_manager->SetStatus(ProviderStatus::kStale);
  ClientAPI client(repo_, "test-domain");

  EXPECT_TRUE(client.GetBooleanValue("flag", false));
}

TEST_F(ClientAPITest, ParallelProviderSwapRaceCondition) {
  std::string domain = "race-domain";
  ClientAPI client(repo_, domain);
  std::atomic<bool> running{true};

  auto ready_provider = std::make_shared<NiceMock<MockFeatureProvider>>();
  ON_CALL(*ready_provider, Init(_)).WillByDefault(Return(absl::OkStatus()));
  ON_CALL(*ready_provider, GetBooleanEvaluation(_, _, _))
      .WillByDefault(testing::Invoke(
          [](std::string_view, bool, const EvaluationContext&)
              -> absl::StatusOr<std::unique_ptr<BoolResolutionDetails>> {
            return std::make_unique<BoolResolutionDetails>(
                true, Reason::kTargetingMatch, std::nullopt, FlagMetadata());
          }));
  repo_.SetProvider(domain, ready_provider,
                    EvaluationContext::Builder().Build(), true);

  std::thread evaluation_thread([&]() {
    while (running) {
      client.GetBooleanValue("flag", false);
      std::this_thread::yield();  // Friendly to single-core CI runners
    }
  });

  auto not_ready_provider = std::make_shared<StrictMock<MockFeatureProvider>>();

  auto init_called = std::make_shared<std::promise<void>>();
  auto proceed_init = std::make_shared<std::promise<void>>();
  std::shared_future<void> proceed_future = proceed_init->get_future().share();

  EXPECT_CALL(*not_ready_provider, Init(_))
      .WillOnce(testing::Invoke([init_called, proceed_future](
                                    const EvaluationContext&) -> absl::Status {
        init_called->set_value();
        proceed_future.wait();
        return absl::OkStatus();
      }));

  EXPECT_CALL(*not_ready_provider, GetBooleanEvaluation(_, _, _)).Times(0);
  EXPECT_CALL(*not_ready_provider, GetHooks())
      .WillRepeatedly(
          Return(std::vector<std::shared_ptr<openfeature::GeneralHook>>{}));
  EXPECT_CALL(*not_ready_provider, GetMetadata())
      .WillRepeatedly(Return(Metadata{"not-ready-mock"}));
  EXPECT_CALL(*not_ready_provider, Shutdown())
      .Times(testing::AtMost(1))
      .WillOnce(Return(absl::OkStatus()));

  repo_.SetProvider(domain, not_ready_provider,
                    EvaluationContext::Builder().Build(), false);

  init_called->get_future().wait();

  std::this_thread::sleep_for(std::chrono::milliseconds(kSleepTimeMs));
  running = false;
  evaluation_thread.join();
  proceed_init->set_value();
}

namespace {
class DummyHook1 : public openfeature::BoolHook {};
class DummyHook2 : public openfeature::StringHook {};
}  // namespace

// Test that client is initialized with empty hooks by default.
TEST_F(ClientAPITest, InitialStateHasEmptyHooks) {
  ClientAPI client(repo_, "test-domain");
  EXPECT_TRUE(client.GetHooks().empty());
}

// Test adding a single hook via AddHook.
TEST_F(ClientAPITest, AddHookAppendsSingleHook) {
  ClientAPI client(repo_, "test-domain");
  std::shared_ptr<openfeature::GeneralHook> hook1 =
      std::make_shared<DummyHook1>();
  client.AddHook(hook1);

  auto hooks = client.GetHooks();
  ASSERT_EQ(hooks.size(), 1);
  EXPECT_EQ(hooks[0], hook1);
}

// Test adding multiple hooks via AddHooks and preserving registration order.
TEST_F(ClientAPITest, AddHooksAppendsMultipleHooksAndPreservesOrder) {
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

  // Adding another hook appends without clearing existing ones
  std::shared_ptr<openfeature::GeneralHook> hook3 =
      std::make_shared<DummyHook1>();
  client.AddHook(hook3);

  hooks = client.GetHooks();
  ASSERT_EQ(hooks.size(), 3);
  EXPECT_EQ(hooks[0], hook1);
  EXPECT_EQ(hooks[1], hook2);
  EXPECT_EQ(hooks[2], hook3);
}

// Test that AddHook and AddHooks filter out nullptr entries.
TEST_F(ClientAPITest, AddHookAndAddHooksFiltersNullptrs) {
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

namespace {

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

}  // namespace

// Test full 4-tier hook execution order on success:
// Before: API -> Client -> Invocation -> Provider
// After: Provider -> Invocation -> Client -> API
// Finally: Provider -> Invocation -> Client -> API
TEST_F(ClientAPITest, HooksExecuteInCorrectOrderOnSuccess) {
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

  GlobalHookManager::GetInstance().AddHook(api_hook);

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

// Test that evaluation context returned by before hooks accumulates and reaches
// provider
TEST_F(ClientAPITest,
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

// Test that HookData is isolated per hook instance and persists across stages
TEST_F(ClientAPITest, HookDataIsIsolatedPerHookAndPersistsAcrossStages) {
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

// Test that HookHints are passed to Before, After, and Finally
TEST_F(ClientAPITest, HookHintsArePropagatedToAllStages) {
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

// Test that an error in Before skips resolution and executes Error and Finally
TEST_F(ClientAPITest,
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

// Test that an error in After mutates details to error state and executes Error
// and Finally
TEST_F(ClientAPITest,
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

// Test that exceptions thrown inside Error or Finally hooks do not prevent
// other hooks from running
TEST_F(ClientAPITest, ExceptionInErrorOrFinallyDoesNotAbortExecution) {
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

  // Client evaluation MUST NOT throw
  EXPECT_NO_THROW({
    auto details = client.GetBooleanDetails("test_flag", false);
    EXPECT_FALSE(details.GetValue());
    EXPECT_EQ(details.GetReason(), Reason::kError);
  });
}

namespace {
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

// Test that type-specific hooks only execute for their matching flag type
TEST_F(ClientAPITest, TypeSpecificHooksExecuteOnlyForMatchingFlagTypes) {
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

  // Now evaluate string flag
  bool_hook_called = false;
  string_hook_called = false;
  client.GetStringValue("string_flag", "default_val");
  EXPECT_TRUE(string_hook_called);
  EXPECT_FALSE(bool_hook_called);
}

// Test that CollectHooks aggregates hooks across all 4 tiers in FIFO order
TEST_F(ClientAPITest, CollectHooksAggregatesAllTiersInPrecedenceOrder) {
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

  GlobalHookManager::GetInstance().AddHook(api_hook);

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

// Test that CollectHooks filters out nullptr hooks at all tiers
TEST_F(ClientAPITest, CollectHooksFiltersNullptrsAcrossAllTiers) {
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

  GlobalHookManager::GetInstance().AddHooks({nullptr, api_hook, nullptr});

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

// Test that CollectHooks handles nullopt options and null provider gracefully
TEST_F(ClientAPITest, CollectHooksHandlesNulloptOptionsAndNullProvider) {
  std::string domain = "collect-hooks-nullopt-domain";

  std::vector<std::string> execution_log;
  auto api_hook = std::make_shared<OrderTrackingHook>("api", execution_log);
  auto client_hook =
      std::make_shared<OrderTrackingHook>("client", execution_log);

  GlobalHookManager::GetInstance().AddHook(api_hook);

  ClientAPI client(repo_, domain);
  client.AddHook(client_hook);

  auto collected_hooks =
      HookSupport::CollectHooks(client.GetHooks(), std::nullopt, nullptr);
  ASSERT_EQ(collected_hooks.size(), 2);
  EXPECT_EQ(collected_hooks[0], api_hook);
  EXPECT_EQ(collected_hooks[1], client_hook);
}

// Test that CreateHookDataMap creates distinct instances per hook and maps
// duplicates to the same instance
TEST_F(ClientAPITest,
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

// Test that when provider is in kNotReady status, Error and Finally hooks
// execute across all tiers (including provider hooks) with kProviderNotReady
TEST_F(ClientAPITest, ProviderNotReadyTriggersErrorAndFinallyHooks) {
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

// Test that when provider is in kFatal status, Error and Finally hooks execute
// across all tiers (including provider hooks) with kProviderFatal
TEST_F(ClientAPITest, ProviderFatalTriggersErrorAndFinallyHooks) {
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
