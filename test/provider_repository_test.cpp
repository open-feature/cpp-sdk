#include "openfeature/provider_repository.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <chrono>
#include <future>

#include "mocks/mock_feature_provider.h"
#include "openfeature/noop_provider.h"

using namespace openfeature;
using ::testing::_;
using ::testing::InSequence;
using ::testing::Return;

class ProviderRepositoryTest : public ::testing::Test {
 protected:
  ProviderRepository repo;
  EvaluationContext ctx;
};

// Test to verify the constructor initializes with a NoopProvider.
TEST_F(ProviderRepositoryTest, ConstructorInitializesWithNoopProvider) {
  std::shared_ptr<FeatureProvider> provider = repo.GetProvider();
  ASSERT_NE(provider, nullptr);
  EXPECT_NE(dynamic_cast<NoopProvider*>(provider.get()), nullptr);
  EXPECT_EQ(repo.GetProvider()->GetMetadata().name, "Noop Provider");
  EXPECT_EQ(repo.GetProviderStatus(), ProviderStatus::kReady);
}

// Test to verify that GetFeatureProviderStatusManager returns the correct
// manager.
TEST_F(ProviderRepositoryTest,
       GetFeatureProviderStatusManagerReturnsCorrectManager) {
  // On initialization, it should return the default manager.
  std::shared_ptr<FeatureProviderStatusManager> initial_default_manager =
      repo.GetFeatureProviderStatusManager();
  ASSERT_NE(initial_default_manager, nullptr);

  // Asking for a non-existent domain should return the default manager.
  std::shared_ptr<FeatureProviderStatusManager> non_existent_manager =
      repo.GetFeatureProviderStatusManager("non-existent");
  EXPECT_EQ(non_existent_manager, initial_default_manager);

  // After setting a named provider, it should return the new manager.
  std::shared_ptr<MockFeatureProvider> mock_provider =
      std::make_shared<MockFeatureProvider>();
  std::string domain = "my-domain";
  EXPECT_CALL(*mock_provider, Init(_)).WillOnce(Return(absl::OkStatus()));
  repo.SetProvider(domain, mock_provider, ctx, true);

  std::shared_ptr<FeatureProviderStatusManager> named_manager =
      repo.GetFeatureProviderStatusManager(domain);
  ASSERT_NE(named_manager, nullptr);
  EXPECT_NE(named_manager, initial_default_manager);
  EXPECT_EQ(named_manager->GetProvider(), mock_provider);

  // Getting the default manager should still return the original one.
  std::shared_ptr<FeatureProviderStatusManager> current_default_manager =
      repo.GetFeatureProviderStatusManager();
  EXPECT_EQ(current_default_manager, initial_default_manager);
}

// Test to verify GetProvider returns the default when the domain is empty.
TEST_F(ProviderRepositoryTest, GetProviderReturnsDefaultWhenDomainIsEmpty) {
  std::shared_ptr<FeatureProvider> default_provider = repo.GetProvider();
  std::shared_ptr<FeatureProvider> empty_domain_provider = repo.GetProvider("");
  EXPECT_EQ(default_provider, empty_domain_provider);
}

// Test to verify GetProvider returns the default when the domain is not found.
TEST_F(ProviderRepositoryTest, GetProviderReturnsDefaultWhenDomainNotFound) {
  std::shared_ptr<FeatureProvider> default_provider = repo.GetProvider();
  std::shared_ptr<FeatureProvider> not_found_provider =
      repo.GetProvider("non-existent-domain");
  EXPECT_EQ(default_provider, not_found_provider);
}

// Test to verify setting and retrieving a named provider.
TEST_F(ProviderRepositoryTest, SetAndGetNamedProvider) {
  std::shared_ptr<MockFeatureProvider> mock_provider =
      std::make_shared<MockFeatureProvider>();
  std::string domain = "my-domain";

  EXPECT_CALL(*mock_provider, Init(_)).WillOnce(Return(absl::OkStatus()));

  repo.SetProvider(domain, mock_provider, ctx, true);

  EXPECT_EQ(repo.GetProvider(domain), mock_provider);
  EXPECT_NE(dynamic_cast<NoopProvider*>(repo.GetProvider().get()), nullptr);
}

// Test to verify that setting the default provider replaces the NoopProvider.
TEST_F(ProviderRepositoryTest, SetDefaultProviderReplacesNoop) {
  std::shared_ptr<MockFeatureProvider> mock_provider =
      std::make_shared<MockFeatureProvider>();

  EXPECT_CALL(*mock_provider, Init(_)).WillOnce(Return(absl::OkStatus()));

  repo.SetProvider(mock_provider, ctx, true);

  EXPECT_EQ(repo.GetProvider(), mock_provider);
}

// Test that setting a null provider for the default does not change the state.
TEST_F(ProviderRepositoryTest, SetProviderWithNullProviderDoesNothing) {
  std::shared_ptr<FeatureProvider> initial_provider = repo.GetProvider();
  ProviderStatus initial_status = repo.GetProviderStatus();
  testing::internal::CaptureStderr();

  repo.SetProvider(nullptr, ctx, true);
  std::string output = testing::internal::GetCapturedStderr();

  EXPECT_THAT(output, testing::HasSubstr("Provider cannot be null"));
  EXPECT_EQ(repo.GetProvider(), initial_provider);
  EXPECT_EQ(repo.GetProviderStatus(), initial_status);
}

// Test that setting a null provider for a named domain does not change the
// state.
TEST_F(ProviderRepositoryTest, SetNamedProviderWithNullProviderDoesNothing) {
  const std::string domain = "my-domain";
  std::shared_ptr<FeatureProvider> initial_provider_for_domain =
      repo.GetProvider(domain);
  ProviderStatus initial_status = repo.GetProviderStatus(domain);

  testing::internal::CaptureStderr();

  repo.SetProvider(domain, nullptr, ctx, true);
  std::string output = testing::internal::GetCapturedStderr();

  EXPECT_THAT(output, testing::HasSubstr("Provider cannot be null"));
  EXPECT_EQ(repo.GetProvider(domain), initial_provider_for_domain);
  EXPECT_EQ(repo.GetProviderStatus(domain), initial_status);
}

// Test to verify that SetProvider waits for initialization to complete.
TEST_F(ProviderRepositoryTest, SetProviderWaitsForInitialization) {
  std::shared_ptr<MockFeatureProvider> mock_provider =
      std::make_shared<MockFeatureProvider>();

  EXPECT_CALL(*mock_provider, Init(_)).WillOnce(Return(absl::OkStatus()));

  repo.SetProvider(mock_provider, ctx, true);

  EXPECT_EQ(repo.GetProviderStatus(), ProviderStatus::kReady);
}

// Test to verify that SetProvider initializes asynchronously.
TEST_F(ProviderRepositoryTest, SetProviderDoesNotWaitForInitialization) {
  std::shared_ptr<MockFeatureProvider> mock_provider =
      std::make_shared<MockFeatureProvider>();

  std::promise<void> init_can_complete;
  std::future<void> init_future = init_can_complete.get_future();

  EXPECT_CALL(*mock_provider, Init(_)).WillOnce([&init_future](const auto&) {
    init_future.get();
    return absl::OkStatus();
  });

  repo.SetProvider(mock_provider, ctx, false);

  EXPECT_EQ(repo.GetProviderStatus(), ProviderStatus::kNotReady);

  init_can_complete.set_value();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  EXPECT_EQ(repo.GetProviderStatus(), ProviderStatus::kReady);
}

// Test to verify that a failed initialization sets the status to kError.
TEST_F(ProviderRepositoryTest, SetProviderWithFailedInitSetsErrorStatus) {
  std::shared_ptr<MockFeatureProvider> mock_provider =
      std::make_shared<MockFeatureProvider>();

  EXPECT_CALL(*mock_provider, Init(_))
      .WillOnce(Return(absl::InternalError("Init failed")));

  repo.SetProvider(mock_provider, ctx, true);

  EXPECT_EQ(repo.GetProviderStatus(), ProviderStatus::kError);
}

// Test that Shutdown resets the repository to its initial
// state.
TEST_F(ProviderRepositoryTest, ShutdownResetsToReadyNoopProvider) {
  // Set a mock provider to ensure we're not in the initial state.
  auto mock_provider = std::make_shared<MockFeatureProvider>();
  EXPECT_CALL(*mock_provider, Init(_)).WillOnce(Return(absl::OkStatus()));
  repo.SetProvider(mock_provider, ctx, true);
  ASSERT_EQ(repo.GetProvider(), mock_provider);

  // Expect the mock provider to be shut down.
  EXPECT_CALL(*mock_provider, Shutdown()).WillOnce(Return(absl::OkStatus()));
  repo.Shutdown();

  // After shutdown, the repository should be reset to the default NoopProvider.
  auto provider = repo.GetProvider();
  ASSERT_NE(provider, nullptr);
  EXPECT_NE(dynamic_cast<NoopProvider*>(provider.get()), nullptr);
  EXPECT_EQ(repo.GetProviderStatus(), ProviderStatus::kReady);
}

// Test to verify the old provider is shutdown after a new one is ready.
TEST_F(ProviderRepositoryTest, OldProviderIsShutdownAfterNewOneIsReady) {
  std::shared_ptr<MockFeatureProvider> mock_provider1 =
      std::make_shared<MockFeatureProvider>();
  std::shared_ptr<MockFeatureProvider> mock_provider2 =
      std::make_shared<MockFeatureProvider>();

  EXPECT_CALL(*mock_provider1, Init(_)).WillOnce(Return(absl::OkStatus()));
  repo.SetProvider(mock_provider1, ctx, true);
  EXPECT_EQ(repo.GetProvider(), mock_provider1);
  ASSERT_EQ(repo.GetProviderStatus(), ProviderStatus::kReady);

  {
    InSequence seq;
    EXPECT_CALL(*mock_provider2, Init(_)).WillOnce(Return(absl::OkStatus()));
    EXPECT_CALL(*mock_provider1, Shutdown()).WillOnce(Return(absl::OkStatus()));
  }

  repo.SetProvider(mock_provider2, ctx, true);
  EXPECT_EQ(repo.GetProvider(), mock_provider2);
  EXPECT_EQ(repo.GetProviderStatus(), ProviderStatus::kReady);
}

// Test to verify the old provider is not shut down if the new one fails to
// init.
TEST_F(ProviderRepositoryTest, OldProviderIsNotShutdownIfNewOneFailsToInit) {
  std::shared_ptr<MockFeatureProvider> mock_provider1 =
      std::make_shared<MockFeatureProvider>();
  std::shared_ptr<MockFeatureProvider> mock_provider2 =
      std::make_shared<MockFeatureProvider>();

  EXPECT_CALL(*mock_provider1, Init(_)).WillOnce(Return(absl::OkStatus()));
  repo.SetProvider(mock_provider1, ctx, true);

  EXPECT_CALL(*mock_provider2, Init(_))
      .WillOnce(Return(absl::InternalError("Init failed")));
  EXPECT_CALL(*mock_provider1, Shutdown()).Times(0);

  repo.SetProvider(mock_provider2, ctx, true);

  // The new provider should be set, but in an error state.
  EXPECT_EQ(repo.GetProvider(), mock_provider2);
  EXPECT_EQ(repo.GetProviderStatus(), ProviderStatus::kError);
}

// Test to verify Shutdown calls Shutdown on all registered providers.
TEST_F(ProviderRepositoryTest, ShutdownAllProviders) {
  std::shared_ptr<MockFeatureProvider> mock_default =
      std::make_shared<MockFeatureProvider>();
  std::shared_ptr<MockFeatureProvider> mock_named_1 =
      std::make_shared<MockFeatureProvider>();
  std::string domain_1 = "my-first-domain";
  std::shared_ptr<MockFeatureProvider> mock_named_2 =
      std::make_shared<MockFeatureProvider>();
  std::string domain_2 = "my-second-domain";
  std::string domain_3 = "my-third-domain";

  EXPECT_CALL(*mock_default, Init(_)).WillOnce(Return(absl::OkStatus()));
  EXPECT_CALL(*mock_named_1, Init(_)).WillOnce(Return(absl::OkStatus()));
  EXPECT_CALL(*mock_named_2, Init(_)).WillOnce(Return(absl::OkStatus()));

  repo.SetProvider(mock_default, ctx, true);
  repo.SetProvider(domain_1, mock_named_1, ctx, true);
  repo.SetProvider(domain_2, mock_named_2, ctx, true);
  repo.SetProvider(domain_3, mock_named_2, ctx, true);

  EXPECT_CALL(*mock_default, Shutdown()).WillOnce(Return(absl::OkStatus()));
  EXPECT_CALL(*mock_named_1, Shutdown()).WillOnce(Return(absl::OkStatus()));
  EXPECT_CALL(*mock_named_2, Shutdown()).WillOnce(Return(absl::OkStatus()));

  // Keep a reference to the old manager.
  std::shared_ptr<FeatureProviderStatusManager> old_manager =
      repo.GetFeatureProviderStatusManager();

  repo.Shutdown();

  // Assert that a new default manager has been created and it's not the old
  // one.
  std::shared_ptr<FeatureProviderStatusManager> new_manager =
      repo.GetFeatureProviderStatusManager();
  ASSERT_NE(new_manager, nullptr);

  ASSERT_NE(new_manager, old_manager);

  // Assert that the new provider is the default NoopProvider.
  std::shared_ptr<FeatureProvider> new_provider = new_manager->GetProvider();
  ASSERT_NE(new_provider, nullptr);
  EXPECT_NE(dynamic_cast<NoopProvider*>(new_provider.get()), nullptr);
}

// Test to verify that Shutdown waits for asynchronous initialization to finish.
TEST_F(ProviderRepositoryTest, ShutdownWaitsForAsyncInitializationToComplete) {
  std::shared_ptr<MockFeatureProvider> mock_provider =
      std::make_shared<MockFeatureProvider>();

  std::promise<void> init_started_promise;
  std::future<void> init_started_future = init_started_promise.get_future();

  std::promise<void> init_can_complete_promise;
  std::future<void> init_can_complete_future =
      init_can_complete_promise.get_future();

  // Mock the Init() call.
  EXPECT_CALL(*mock_provider, Init(_)).WillOnce([&](const auto&) {
    init_started_promise.set_value();
    init_can_complete_future.get();
    return absl::OkStatus();
  });

  EXPECT_CALL(*mock_provider, Shutdown()).WillOnce(Return(absl::OkStatus()));

  repo.SetProvider(mock_provider, ctx, false);

  // Wait until the background thread is confirmed to be inside the Init()
  // method.
  init_started_future.wait();

  // Run Shutdown() to verify that it blocks until init is allowed to complete.
  auto shutdown_future =
      std::async(std::launch::async, [&]() { repo.Shutdown(); });

  // We expect it to time out because the Init() is still blocked.
  auto status = shutdown_future.wait_for(std::chrono::milliseconds(100));
  ASSERT_EQ(status, std::future_status::timeout)
      << "Shutdown() did not wait for initialization to complete.";

  // Unblock Init().
  init_can_complete_promise.set_value();

  // Shutdown should complete promptly.
  shutdown_future.get();

  SUCCEED() << "Shutdown() correctly waited for the async task.";
}

// Test that setting an existing provider in a new location reuses the manager.
TEST_F(ProviderRepositoryTest, SetExistingProviderReusesManager) {
  std::shared_ptr<MockFeatureProvider> mock_provider =
      std::make_shared<MockFeatureProvider>();
  std::string domain = "my-domain";

  EXPECT_CALL(*mock_provider, Init(_)).WillOnce(Return(absl::OkStatus()));

  repo.SetProvider(mock_provider, ctx, true);
  std::shared_ptr<FeatureProviderStatusManager> default_manager =
      repo.GetFeatureProviderStatusManager();

  repo.SetProvider(domain, mock_provider, ctx, true);
  std::shared_ptr<FeatureProviderStatusManager> named_manager =
      repo.GetFeatureProviderStatusManager(domain);

  EXPECT_EQ(default_manager, named_manager);
}

// Test that replacing a provider does not shut it down if it's still in use.
TEST_F(ProviderRepositoryTest, ReplacingProviderDoesNotShutdownIfStillInUse) {
  std::shared_ptr<MockFeatureProvider> mock_provider1 =
      std::make_shared<MockFeatureProvider>();
  std::shared_ptr<MockFeatureProvider> mock_provider2 =
      std::make_shared<MockFeatureProvider>();
  std::string domain_a = "domain-A";

  EXPECT_CALL(*mock_provider1, Init(_)).WillOnce(Return(absl::OkStatus()));
  EXPECT_CALL(*mock_provider2, Init(_)).WillOnce(Return(absl::OkStatus()));

  repo.SetProvider(mock_provider1, ctx, true);
  repo.SetProvider(domain_a, mock_provider1, ctx, true);

  std::shared_ptr<FeatureProviderStatusManager> manager_for_provider1 =
      repo.GetFeatureProviderStatusManager();
  ASSERT_EQ(manager_for_provider1->GetProvider(), mock_provider1);

  EXPECT_CALL(*mock_provider1, Shutdown()).Times(0);
  repo.SetProvider(mock_provider2, ctx, true);

  EXPECT_EQ(manager_for_provider1->GetStatus(), ProviderStatus::kReady);

  EXPECT_CALL(*mock_provider1, Shutdown()).WillOnce(Return(absl::OkStatus()));
  repo.SetProvider(domain_a, mock_provider2, ctx, true);

  EXPECT_EQ(manager_for_provider1->GetStatus(), ProviderStatus::kNotReady);
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
