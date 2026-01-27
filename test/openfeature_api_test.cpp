#include "openfeature/openfeature_api.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <future>
#include <thread>

#include "mocks/mock_feature_provider.h"
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
    api.SetEvaluationContext(EvaluationContext{});
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

  EXPECT_CALL(*mock_provider, Init(_)).WillOnce([&](const auto&) {
    init_can_start.set_value();
    init_can_complete_future.wait();
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

  EXPECT_CALL(*mock_provider, Init(_)).WillOnce([&](const auto&) {
    init_can_start.set_value();
    init_can_complete_future.wait();
    return absl::OkStatus();
  });
  EXPECT_CALL(*mock_provider, Shutdown()).WillOnce(Return(absl::OkStatus()));
  api.SetProvider(domain, mock_provider);

  // Confirm the background task has started.
  auto status = init_started_future.wait_for(std::chrono::seconds(1));
  ASSERT_EQ(status, std::future_status::ready)
      << "Async initialization did not start for named provider.";

  // Allow the init to complete.
  init_can_complete.set_value();
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

// TODO: Add tests for "GetEvaluationContext" and "SetEvaluationContext" once.
// EvaluationContext logic is implemented.
