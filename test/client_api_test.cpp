#include "openfeature/client_api.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <future>
#include <memory>
#include <string>
#include <thread>

#include "absl/status/status.h"
#include "mocks/mock_feature_provider.h"
#include "openfeature/evaluation_context.h"
#include "openfeature/evaluation_options.h"
#include "openfeature/global_context_manager.h"
#include "openfeature/hook.h"
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
    // Reset the Global Context to a clean state before each test.
    GlobalContextManager::GetInstance().SetGlobalEvaluationContext(
        EvaluationContext::Builder().build());
  }
  ProviderRepository repo_;
};

constexpr int kUnknownExceptionError = 43;
constexpr int kSleepTimeMs = 10;

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
  EvaluationContext ctx = EvaluationContext::Builder().build();

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
  EXPECT_EQ(client.GetIntegerValue(flag_key, 42), 42);
}

// Test that GetDoubleValue returns the default value when using the default
// provider.
TEST_F(ClientAPITest, GetDoubleValueReturnsDefaultWithNoopProvider) {
  ClientAPI client(repo_, "test-domain");
  std::string flag_key = "my-double-flag";
  EXPECT_DOUBLE_EQ(client.GetDoubleValue(flag_key, 3.14), 3.14);
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
  EvaluationContext ctx = EvaluationContext::Builder().build();
  std::string flag_key = "my-boolean-flag";

  EXPECT_TRUE(client.GetBooleanValue(flag_key, true, ctx));
  EXPECT_FALSE(client.GetBooleanValue(flag_key, false, ctx));
}

// Test GetStringValue with an EvaluationContext passed in.
TEST_F(ClientAPITest, GetStringValueWithContextReturnsDefault) {
  ClientAPI client(repo_, "test-domain");
  EvaluationContext ctx = EvaluationContext::Builder().build();
  std::string flag_key = "my-string-flag";
  EXPECT_EQ(client.GetStringValue(flag_key, "default", ctx), "default");
}

// Test that GetIntegerValue with an EvaluationContext passed in.
TEST_F(ClientAPITest, GetIntegerValueWithContextReturnsDefault) {
  ClientAPI client(repo_, "test-domain");
  EvaluationContext ctx = EvaluationContext::Builder().build();
  std::string flag_key = "my-integer-flag";
  EXPECT_EQ(client.GetIntegerValue(flag_key, 42, ctx), 42);
}

// Test that GetDoubleValue with an EvaluationContext passed in.
TEST_F(ClientAPITest, GetDoubleValueWithContextReturnsDefault) {
  ClientAPI client(repo_, "test-domain");
  EvaluationContext ctx = EvaluationContext::Builder().build();
  std::string flag_key = "my-double-flag";
  EXPECT_DOUBLE_EQ(client.GetDoubleValue(flag_key, 3.14, ctx), 3.14);
}

// Test that GetObjectValue with an EvaluationContext passed in.
TEST_F(ClientAPITest, GetObjectValueWithContextReturnsDefault) {
  ClientAPI client(repo_, "test-domain");
  EvaluationContext ctx = EvaluationContext::Builder().build();
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

  EvaluationContext ctx = EvaluationContext::Builder().build();
  EXPECT_TRUE(client.GetBooleanValue(flag_key, true, ctx, options));
  EXPECT_FALSE(client.GetBooleanValue(flag_key, false, ctx, options));
}

// Test GetStringValue with EvaluationOptions.
TEST_F(ClientAPITest, GetStringValueWithOptionsReturnsDefault) {
  ClientAPI client(repo_, "test-domain");
  EvaluationOptions options;
  std::string flag_key = "my-string-flag";

  EXPECT_EQ(client.GetStringValue(flag_key, "default", options), "default");

  EvaluationContext ctx = EvaluationContext::Builder().build();
  EXPECT_EQ(client.GetStringValue(flag_key, "default", ctx, options),
            "default");
}

// Test GetIntegerValue with EvaluationOptions.
TEST_F(ClientAPITest, GetIntegerValueWithOptionsReturnsDefault) {
  ClientAPI client(repo_, "test-domain");
  EvaluationOptions options;
  std::string flag_key = "my-integer-flag";

  EXPECT_EQ(client.GetIntegerValue(flag_key, 42, options), 42);

  EvaluationContext ctx = EvaluationContext::Builder().build();
  EXPECT_EQ(client.GetIntegerValue(flag_key, 42, ctx, options), 42);
}

// Test GetDoubleValue with EvaluationOptions.
TEST_F(ClientAPITest, GetDoubleValueWithOptionsReturnsDefault) {
  ClientAPI client(repo_, "test-domain");
  EvaluationOptions options;
  std::string flag_key = "my-double-flag";

  EXPECT_DOUBLE_EQ(client.GetDoubleValue(flag_key, 3.14, options), 3.14);

  EvaluationContext ctx = EvaluationContext::Builder().build();
  EXPECT_DOUBLE_EQ(client.GetDoubleValue(flag_key, 3.14, ctx, options), 3.14);
}

// Test GetObjectValue with EvaluationOptions.
TEST_F(ClientAPITest, GetObjectValueWithOptionsReturnsDefault) {
  ClientAPI client(repo_, "test-domain");
  EvaluationOptions options;
  std::string flag_key = "my-object-flag";

  EXPECT_EQ(client.GetObjectValue(flag_key, Value(1), options), Value(1));

  EvaluationContext ctx = EvaluationContext::Builder().build();
  EXPECT_EQ(client.GetObjectValue(flag_key, Value(1), ctx, options), Value(1));
}

// Test GetBooleanDetails returns proper details with NoopProvider.
TEST_F(ClientAPITest, GetBooleanDetailsReturnsDetailsWithNoopProvider) {
  ClientAPI client(repo_, "test-domain");
  std::string flag_key = "my-boolean-flag";
  EvaluationContext ctx = EvaluationContext::Builder().build();
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
  EvaluationContext ctx = EvaluationContext::Builder().build();
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
  EvaluationContext ctx = EvaluationContext::Builder().build();
  EvaluationOptions options;

  auto details1 = client.GetIntegerDetails(flag_key, 42);
  EXPECT_EQ(details1.GetFlagKey(), flag_key);
  EXPECT_EQ(details1.GetValue(), 42);
  EXPECT_EQ(details1.GetReason(), Reason::kDefault);
  EXPECT_EQ(details1.GetErrorCode(), std::nullopt);

  auto details2 = client.GetIntegerDetails(flag_key, 42, ctx);
  EXPECT_EQ(details2.GetFlagKey(), flag_key);
  EXPECT_EQ(details2.GetValue(), 42);

  auto details3 = client.GetIntegerDetails(flag_key, 42, options);
  EXPECT_EQ(details3.GetFlagKey(), flag_key);
  EXPECT_EQ(details3.GetValue(), 42);

  auto details4 = client.GetIntegerDetails(flag_key, 42, ctx, options);
  EXPECT_EQ(details4.GetFlagKey(), flag_key);
  EXPECT_EQ(details4.GetValue(), 42);
}

// Test GetDoubleDetails returns proper details with NoopProvider.
TEST_F(ClientAPITest, GetDoubleDetailsReturnsDetailsWithNoopProvider) {
  ClientAPI client(repo_, "test-domain");
  std::string flag_key = "my-double-flag";
  EvaluationContext ctx = EvaluationContext::Builder().build();
  EvaluationOptions options;

  auto details1 = client.GetDoubleDetails(flag_key, 3.14);
  EXPECT_EQ(details1.GetFlagKey(), flag_key);
  EXPECT_DOUBLE_EQ(details1.GetValue(), 3.14);
  EXPECT_EQ(details1.GetReason(), Reason::kDefault);
  EXPECT_EQ(details1.GetErrorCode(), std::nullopt);

  auto details2 = client.GetDoubleDetails(flag_key, 3.14, ctx);
  EXPECT_EQ(details2.GetFlagKey(), flag_key);
  EXPECT_DOUBLE_EQ(details2.GetValue(), 3.14);

  auto details3 = client.GetDoubleDetails(flag_key, 3.14, options);
  EXPECT_EQ(details3.GetFlagKey(), flag_key);
  EXPECT_DOUBLE_EQ(details3.GetValue(), 3.14);

  auto details4 = client.GetDoubleDetails(flag_key, 3.14, ctx, options);
  EXPECT_EQ(details4.GetFlagKey(), flag_key);
  EXPECT_DOUBLE_EQ(details4.GetValue(), 3.14);
}

// Test GetObjectDetails returns proper details with NoopProvider.
TEST_F(ClientAPITest, GetObjectDetailsReturnsDetailsWithNoopProvider) {
  ClientAPI client(repo_, "test-domain");
  std::string flag_key = "my-object-flag";
  EvaluationContext ctx = EvaluationContext::Builder().build();
  EvaluationOptions options;

  auto details1 = client.GetObjectDetails(flag_key, Value(100));
  EXPECT_EQ(details1.GetFlagKey(), flag_key);
  EXPECT_EQ(details1.GetValue(), Value(100));
  EXPECT_EQ(details1.GetReason(), Reason::kDefault);
  EXPECT_EQ(details1.GetErrorCode(), std::nullopt);

  auto details2 = client.GetObjectDetails(flag_key, Value(100), ctx);
  EXPECT_EQ(details2.GetFlagKey(), flag_key);
  EXPECT_EQ(details2.GetValue(), Value(100));

  auto details3 = client.GetObjectDetails(flag_key, Value(100), options);
  EXPECT_EQ(details3.GetFlagKey(), flag_key);
  EXPECT_EQ(details3.GetValue(), Value(100));

  auto details4 = client.GetObjectDetails(flag_key, Value(100), ctx, options);
  EXPECT_EQ(details4.GetFlagKey(), flag_key);
  EXPECT_EQ(details4.GetValue(), Value(100));
}

// Test context merging logic indirectly.
TEST_F(ClientAPITest, ContextMergingPrecedence) {
  GlobalContextManager::GetInstance().SetGlobalEvaluationContext(
      EvaluationContext::Builder()
          .WithTargetingKey("global-target")
          .WithAttribute("global_attr", "global_value")
          .WithAttribute("shared_attr_gc", "global_shared_gc")
          .WithAttribute("shared_attr_gci", "global_shared_gci")
          .build());

  std::shared_ptr<StrictMock<MockFeatureProvider>> mock_provider =
      std::make_shared<StrictMock<MockFeatureProvider>>();

  EXPECT_CALL(*mock_provider, Init(_)).WillOnce(Return(absl::OkStatus()));
  EXPECT_CALL(*mock_provider, Shutdown()).WillOnce(Return(absl::OkStatus()));

  EvaluationContext provider_init_ctx = EvaluationContext::Builder().build();
  repo_.SetProvider("test-domain", mock_provider, provider_init_ctx, true);

  ClientAPI client(repo_, "test-domain");
  client.SetEvaluationContext(
      EvaluationContext::Builder()
          .WithTargetingKey("client-target")
          .WithAttribute("client_attr", "client_value")
          .WithAttribute("shared_attr_gc", "client_shared_gc")
          .WithAttribute("shared_attr_gci", "client_shared_gci")
          .build());

  EvaluationContext invocation_ctx =
      EvaluationContext::Builder()
          .WithTargetingKey("invocation-target")
          .WithAttribute("invocation_attr", "invocation_value")
          .WithAttribute("shared_attr_gci", "invocation_shared_gci")
          .build();

  std::string flag_key = "my-test-flag";
  bool default_value = false;
  bool expected_value = true;

  EvaluationContext captured_merged_ctx = EvaluationContext::Builder().build();

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
                    EvaluationContext::Builder().build(), true);
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
                    EvaluationContext::Builder().build(), true);
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
                    EvaluationContext::Builder().build(), true);
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
                    EvaluationContext::Builder().build(), true);
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
                    EvaluationContext::Builder().build(), true);
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
                    EvaluationContext::Builder().build(), true);
  ClientAPI client(repo_, "test-domain");

  EXPECT_FALSE(client.GetBooleanValue("flag", false));
  EXPECT_TRUE(client.GetBooleanValue("flag", true));
}

TEST_F(ClientAPITest, EvaluateFlagBlocksWhenProviderNotReady) {
  auto mock_provider = std::make_shared<NiceMock<MockFeatureProvider>>();
  EXPECT_CALL(*mock_provider, GetBooleanEvaluation(_, _, _)).Times(0);
  repo_.SetProvider("test-domain", mock_provider,
                    EvaluationContext::Builder().build(), true);

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
                    EvaluationContext::Builder().build(), true);

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
                    EvaluationContext::Builder().build(), true);

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
                    EvaluationContext::Builder().build(), true);

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
                    EvaluationContext::Builder().build(), true);

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
  EXPECT_CALL(*not_ready_provider, Shutdown())
      .Times(testing::AtMost(1))
      .WillOnce(Return(absl::OkStatus()));

  repo_.SetProvider(domain, not_ready_provider,
                    EvaluationContext::Builder().build(), false);

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
