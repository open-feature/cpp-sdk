#include "openfeature/feature_provider_status_manager.h"

#include <gtest/gtest.h>

#include <memory>
#include <tuple>

#include "absl/status/status.h"
#include "mocks/mock_feature_provider.h"
#include "openfeature/evaluation_context.h"
#include "openfeature/provider_status.h"

using namespace openfeature;
using ::testing::_;
using ::testing::Return;
using ::testing::Test;
using ::testing::WithParamInterface;

class FeatureProviderStatusManagerTest : public Test {
 protected:
  void SetUp() override {
    mock_provider = std::make_shared<MockFeatureProvider>();
    auto result = FeatureProviderStatusManager::Create(mock_provider);
    ASSERT_TRUE(result.ok());
    manager = std::move(result).value();
    ASSERT_NE(manager, nullptr);
  }

  std::shared_ptr<MockFeatureProvider> mock_provider;
  std::unique_ptr<FeatureProviderStatusManager> manager;
  EvaluationContext ctx;
};

TEST_F(FeatureProviderStatusManagerTest, CreateWithNullProviderReturnsError) {
  auto result = FeatureProviderStatusManager::Create(nullptr);
  ASSERT_FALSE(result.ok());
  EXPECT_EQ(result.status().code(), absl::StatusCode::kInvalidArgument);
}

TEST_F(FeatureProviderStatusManagerTest, CreateWithValidProviderSucceeds) {
  EXPECT_EQ(manager->GetStatus(), ProviderStatus::kNotReady);
  EXPECT_EQ(manager->GetProvider(), mock_provider);
}

class FeatureProviderStatusManagerInitTest
    : public FeatureProviderStatusManagerTest,
      public WithParamInterface<std::tuple<absl::Status, ProviderStatus>> {};

TEST_P(FeatureProviderStatusManagerInitTest, InitSetsCorrectStatus) {
  auto [provider_init_status, expected_manager_status] = GetParam();

  EXPECT_CALL(*mock_provider, Init(_)).WillOnce(Return(provider_init_status));
  manager->Init(ctx);
  EXPECT_EQ(manager->GetStatus(), expected_manager_status);
}

INSTANTIATE_TEST_SUITE_P(
    InitStatusTests, FeatureProviderStatusManagerInitTest,
    ::testing::Values(
        std::make_tuple(absl::OkStatus(), ProviderStatus::kReady),
        std::make_tuple(absl::InternalError("Initialization failed"),
                        ProviderStatus::kError)));

class FeatureProviderStatusManagerShutdownTest
    : public FeatureProviderStatusManagerTest,
      public WithParamInterface<absl::Status> {};

TEST_P(FeatureProviderStatusManagerShutdownTest,
       ShutdownAlwaysSetsStatusToNotReady) {
  manager->SetStatus(ProviderStatus::kReady);
  auto provider_shutdown_status = GetParam();
  EXPECT_CALL(*mock_provider, Shutdown())
      .WillOnce(Return(provider_shutdown_status));
  manager->Shutdown();
  EXPECT_EQ(manager->GetStatus(), ProviderStatus::kNotReady);
}

INSTANTIATE_TEST_SUITE_P(
    ShutdownStatusTests, FeatureProviderStatusManagerShutdownTest,
    ::testing::Values(absl::OkStatus(),
                      absl::InternalError("Shutdown failed")));

TEST_F(FeatureProviderStatusManagerTest, SetStatusAndGetStatusWorkCorrectly) {
  // Verify initial status from SetUp.
  ASSERT_EQ(manager->GetStatus(), ProviderStatus::kNotReady);

  // Define a list of statuses to test.
  const std::vector<ProviderStatus> statuses_to_test = {
      ProviderStatus::kStale, ProviderStatus::kFatal, ProviderStatus::kReady,
      ProviderStatus::kNotReady, ProviderStatus::kError};

  // Iterate and test each status.
  for (const auto& status : statuses_to_test) {
    manager->SetStatus(status);
    EXPECT_EQ(manager->GetStatus(), status);
  }
}

TEST_F(FeatureProviderStatusManagerTest, GetProviderReturnsCorrectProvider) {
  // The GetProvider method should return the same instance of the provider
  // regardless of the manager's status.
  EXPECT_EQ(manager->GetProvider(), mock_provider);
  manager->SetStatus(ProviderStatus::kReady);
  EXPECT_EQ(manager->GetProvider(), mock_provider);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}