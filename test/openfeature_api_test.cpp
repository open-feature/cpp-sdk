#include "openfeature/openfeature_api.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <future>
#include <thread>

#include "mocks/mock_feature_provider.h"
#include "openfeature/hook.h"
#include "openfeature/noop_provider.h"

using namespace openfeature;
using ::testing::_;
using ::testing::Return;

class OpenFeatureAPITest : public ::testing::Test {
 protected:
  // To ensure test isolation for the singleton, we shut it down before and
  // after each test, to reset it to its default state.
  void SetUp() override {}
  void TearDown() override {
    api.Shutdown();
    api.SetEvaluationContext(EvaluationContext::Builder().build());
  }

  OpenFeatureAPI& api = OpenFeatureAPI::GetInstance();
};

// Test that GetInstance always returns the same singleton instance.
TEST_F(OpenFeatureAPITest, GetInstanceReturnsSameInstance) {
  OpenFeatureAPI& instance1 = OpenFeatureAPI::GetInstance();
  OpenFeatureAPI& instance2 = OpenFeatureAPI::GetInstance();
  ASSERT_EQ(&instance1, &instance2);
}

// Test that the API is initialized with a NoopProvider by default.
TEST_F(OpenFeatureAPITest, InitialStateHasNoopProvider) {
  std::shared_ptr<FeatureProvider> provider = api.GetProvider();
  ASSERT_NE(provider, nullptr);
  EXPECT_NE(dynamic_cast<NoopProvider*>(provider.get()), nullptr);
}

// Test setting the default provider and waiting for its initialization.
TEST_F(OpenFeatureAPITest, SetAndGetDefaultProviderAndWait) {
  std::shared_ptr<MockFeatureProvider> mock_provider =
      std::make_shared<MockFeatureProvider>();
  EXPECT_CALL(*mock_provider, Init(_)).WillOnce(Return(absl::OkStatus()));

  api.SetProviderAndWait(mock_provider);

  EXPECT_EQ(api.GetProvider(), mock_provider);
}

// Test setting a named provider and waiting for its initialization.
TEST_F(OpenFeatureAPITest, SetAndGetNamedProviderAndWait) {
  std::shared_ptr<MockFeatureProvider> mock_provider =
      std::make_shared<MockFeatureProvider>();
  std::string domain = "test-domain";
  EXPECT_CALL(*mock_provider, Init(_)).WillOnce(Return(absl::OkStatus()));

  api.SetProviderAndWait(domain, mock_provider);

  EXPECT_EQ(api.GetProvider(domain), mock_provider);
  EXPECT_NE(dynamic_cast<NoopProvider*>(api.GetProvider().get()), nullptr);
}

// Test that getting a provider for a non-existent domain falls back to the
// default.
TEST_F(OpenFeatureAPITest, GetProviderFallsBackToDefault) {
  std::shared_ptr<FeatureProvider> default_provider = api.GetProvider();
  std::shared_ptr<FeatureProvider> unknown_domain_provider =
      api.GetProvider("unknown-domain");
  EXPECT_EQ(default_provider, unknown_domain_provider);
}

// Test getting metadata from the default provider.
TEST_F(OpenFeatureAPITest, GetProviderMetadataForDefault) {
  Metadata metadata = api.GetProviderMetadata();
  EXPECT_EQ(metadata.name, "Noop Provider");
}

// Test getting metadata from a named provider.
TEST_F(OpenFeatureAPITest, GetProviderMetadataForNamed) {
  std::shared_ptr<MockFeatureProvider> mock_provider =
      std::make_shared<MockFeatureProvider>();
  std::string domain = "metadata-domain";
  Metadata expected_metadata;
  expected_metadata.name = "Mock Provider";

  EXPECT_CALL(*mock_provider, GetMetadata())
      .WillOnce(Return(expected_metadata));
  EXPECT_CALL(*mock_provider, Init(_)).WillOnce(Return(absl::OkStatus()));

  api.SetProviderAndWait(domain, mock_provider);
  Metadata actual_metadata = api.GetProviderMetadata(domain);

  EXPECT_EQ(actual_metadata.name, expected_metadata.name);
}

// Test that the Shutdown method calls Shutdown on all registered providers.
TEST_F(OpenFeatureAPITest, ShutdownCallsProviderShutdown) {
  std::shared_ptr<MockFeatureProvider> mock_default_provider =
      std::make_shared<MockFeatureProvider>();
  std::shared_ptr<MockFeatureProvider> mock_named_provider =
      std::make_shared<MockFeatureProvider>();
  std::string domain = "shutdown-domain";

  EXPECT_CALL(*mock_default_provider, Init(_))
      .WillOnce(Return(absl::OkStatus()));
  EXPECT_CALL(*mock_named_provider, Init(_)).WillOnce(Return(absl::OkStatus()));

  api.SetProviderAndWait(mock_default_provider);
  api.SetProviderAndWait(domain, mock_named_provider);

  EXPECT_CALL(*mock_default_provider, Shutdown())
      .WillOnce(Return(absl::OkStatus()));
  EXPECT_CALL(*mock_named_provider, Shutdown())
      .WillOnce(Return(absl::OkStatus()));

  api.Shutdown();

  testing::Mock::VerifyAndClearExpectations(mock_default_provider.get());
  testing::Mock::VerifyAndClearExpectations(mock_named_provider.get());
}

// Test the asynchronous SetProvider to ensure it doesn't block.
TEST_F(OpenFeatureAPITest, SetProviderAsyncDoesNotBlock) {
  std::shared_ptr<MockFeatureProvider> mock_provider =
      std::make_shared<MockFeatureProvider>();
  std::promise<void> init_can_start;
  std::future<void> init_started_future = init_can_start.get_future();
  std::promise<void> init_can_complete;
  std::future<void> init_can_complete_future = init_can_complete.get_future();
  std::promise<void> init_has_finished;

  EXPECT_CALL(*mock_provider, Init(_)).WillOnce([&](const auto&) {
    init_can_start.set_value();
    init_can_complete_future.wait();
    init_has_finished.set_value();
    return absl::OkStatus();
  });

  EXPECT_CALL(*mock_provider, Shutdown()).WillOnce(Return(absl::OkStatus()));
  api.SetProvider(mock_provider);

  // Confirm the background task has started.
  auto status = init_started_future.wait_for(std::chrono::seconds(1));
  ASSERT_EQ(status, std::future_status::ready)
      << "Async initialization did not start.";

  // Allow the init to complete.
  init_can_complete.set_value();
  init_has_finished.get_future().wait();
}

// Test the asynchronous SetProvider for a named provider to ensure it doesn't
// block.
TEST_F(OpenFeatureAPITest, SetNamedProviderAsyncDoesNotBlock) {
  std::shared_ptr<MockFeatureProvider> mock_provider =
      std::make_shared<MockFeatureProvider>();
  std::string domain = "async-domain";
  std::promise<void> init_can_start;
  std::future<void> init_started_future = init_can_start.get_future();
  std::promise<void> init_can_complete;
  std::future<void> init_can_complete_future = init_can_complete.get_future();
  std::promise<void> init_has_finished;

  EXPECT_CALL(*mock_provider, Init(_)).WillOnce([&](const auto&) {
    init_can_start.set_value();
    init_can_complete_future.wait();
    init_has_finished.set_value();
    return absl::OkStatus();
    ;
  });
  EXPECT_CALL(*mock_provider, Shutdown()).WillOnce(Return(absl::OkStatus()));
  api.SetProvider(domain, mock_provider);

  // Confirm the background task has started.
  auto status = init_started_future.wait_for(std::chrono::seconds(1));
  ASSERT_EQ(status, std::future_status::ready)
      << "Async initialization did not start for named provider.";

  // Allow the init to complete.
  init_can_complete.set_value();
  init_has_finished.get_future().wait();
}

// Test that GetClient returns a valid default ClientAPI instance.
TEST_F(OpenFeatureAPITest, GetDefaultClient) {
  std::shared_ptr<Client> client = api.GetClient();
  EXPECT_NE(client, nullptr) << "GetClient() should return a valid ptr";
  EXPECT_EQ(client->GetMetadata().name, "");
}

// Test that GetClient returns a valid named ClientAPI instance.
TEST_F(OpenFeatureAPITest, GetNamedClient) {
  std::shared_ptr<Client> named_client = api.GetClient("some-domain");
  EXPECT_NE(named_client, nullptr)
      << "GetClient(domain) should return a valid ptr";
  EXPECT_EQ(named_client->GetMetadata().name, "some-domain");
}

// Test that default global evaluation context is empty.
TEST_F(OpenFeatureAPITest, DefaultEvaluationContextIsEmpty) {
  EvaluationContext ctx = api.GetEvaluationContext();
  EXPECT_FALSE(ctx.GetTargetingKey().has_value());
  EXPECT_TRUE(ctx.GetAttributes().empty());
}

// Test setting and retrieving the global evaluation context.
TEST_F(OpenFeatureAPITest, SetAndGetGlobalEvaluationContext) {
  EvaluationContext new_ctx =
      EvaluationContext::Builder()
          .WithTargetingKey("global-user-123")
          .WithAttribute("environment", std::string("production"))
          .WithAttribute("app_version", std::string("2.1.0"))
          .build();

  api.SetEvaluationContext(new_ctx);

  EvaluationContext retrieved_ctx = api.GetEvaluationContext();
  ASSERT_TRUE(retrieved_ctx.GetTargetingKey().has_value());
  EXPECT_EQ(retrieved_ctx.GetTargetingKey().value(), "global-user-123");

  const std::any* env_val = retrieved_ctx.GetValue("environment");
  ASSERT_NE(env_val, nullptr);
  EXPECT_EQ(std::any_cast<std::string>(*env_val), "production");

  const std::any* version_val = retrieved_ctx.GetValue("app_version");
  ASSERT_NE(version_val, nullptr);
  EXPECT_EQ(std::any_cast<std::string>(*version_val), "2.1.0");
}

// Test overwriting an existing global evaluation context.
TEST_F(OpenFeatureAPITest, OverwriteGlobalEvaluationContext) {
  EvaluationContext first_ctx =
      EvaluationContext::Builder().WithTargetingKey("user-initial").build();
  api.SetEvaluationContext(first_ctx);
  EXPECT_EQ(api.GetEvaluationContext().GetTargetingKey().value(),
            "user-initial");

  EvaluationContext updated_ctx =
      EvaluationContext::Builder().WithTargetingKey("user-updated").build();
  api.SetEvaluationContext(updated_ctx);
  EXPECT_EQ(api.GetEvaluationContext().GetTargetingKey().value(),
            "user-updated");
}

namespace {
class DummyHook1 : public BoolHook {};
class DummyHook2 : public StringHook {};
}  // namespace

// Test that GetHooks returns an empty vector initially.
TEST_F(OpenFeatureAPITest, InitialStateHasEmptyHooks) {
  EXPECT_TRUE(api.GetHooks().empty());
}

// Test adding a single hook via AddHook.
TEST_F(OpenFeatureAPITest, AddHookAppendsSingleHook) {
  std::shared_ptr<BaseHook> hook1 = std::make_shared<DummyHook1>();
  api.AddHook(hook1);

  auto hooks = api.GetHooks();
  ASSERT_EQ(hooks.size(), 1);
  EXPECT_EQ(hooks[0], hook1);
}

// Test adding multiple hooks via AddHooks and preserving registration order.
TEST_F(OpenFeatureAPITest, AddHooksAppendsMultipleHooksAndPreservesOrder) {
  std::shared_ptr<BaseHook> hook1 = std::make_shared<DummyHook1>();
  std::shared_ptr<BaseHook> hook2 = std::make_shared<DummyHook2>();

  api.AddHooks({hook1, hook2});

  auto hooks = api.GetHooks();
  ASSERT_EQ(hooks.size(), 2);
  EXPECT_EQ(hooks[0], hook1);
  EXPECT_EQ(hooks[1], hook2);

  // Adding another hook appends without clearing existing ones (Req 1.1.4)
  std::shared_ptr<BaseHook> hook3 = std::make_shared<DummyHook1>();
  api.AddHook(hook3);

  hooks = api.GetHooks();
  ASSERT_EQ(hooks.size(), 3);
  EXPECT_EQ(hooks[0], hook1);
  EXPECT_EQ(hooks[1], hook2);
  EXPECT_EQ(hooks[2], hook3);
}

// Test fetching status for default and named providers.
TEST_F(OpenFeatureAPITest, GetProviderStatusDefaultAndNamed) {
  // Default provider (NoopProvider) is READY upon initialization
  EXPECT_EQ(api.GetProviderStatus(), ProviderStatus::kReady);

  std::shared_ptr<MockFeatureProvider> mock_provider =
      std::make_shared<MockFeatureProvider>();
  std::string domain = "status-domain";

  EXPECT_CALL(*mock_provider, Init(_)).WillOnce(Return(absl::OkStatus()));

  // Prior to registration, provider status for unknown domain returns default
  // status (kReady)
  EXPECT_EQ(api.GetProviderStatus(domain), ProviderStatus::kReady);

  api.SetProviderAndWait(domain, mock_provider);

  EXPECT_EQ(api.GetProviderStatus(domain), ProviderStatus::kReady);
}

// Test that AddHook and AddHooks filter out nullptr entries.
TEST_F(OpenFeatureAPITest, AddHookAndAddHooksFiltersNullptrs) {
  api.AddHook(nullptr);
  EXPECT_TRUE(api.GetHooks().empty());

  std::shared_ptr<BaseHook> valid_hook = std::make_shared<DummyHook1>();
  api.AddHooks({nullptr, valid_hook, nullptr});

  auto hooks = api.GetHooks();
  ASSERT_EQ(hooks.size(), 1);
  EXPECT_EQ(hooks[0], valid_hook);
}

// Test that Shutdown clears all registered global hooks (Req 1.6.2).
TEST_F(OpenFeatureAPITest, ShutdownClearsAllGlobalHooks) {
  std::shared_ptr<BaseHook> hook1 = std::make_shared<DummyHook1>();
  std::shared_ptr<BaseHook> hook2 = std::make_shared<DummyHook2>();
  api.AddHooks({hook1, hook2});

  ASSERT_EQ(api.GetHooks().size(), 2);

  api.Shutdown();

  EXPECT_TRUE(api.GetHooks().empty())
      << "Shutdown must clear all registered global hooks.";
}
