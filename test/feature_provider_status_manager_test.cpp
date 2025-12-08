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
    mock_provider_ = std::make_shared<MockFeatureProvider>();
    absl::StatusOr<std::unique_ptr<FeatureProviderStatusManager>> result =
        FeatureProviderStatusManager::Create(mock_provider_);
    ASSERT_TRUE(result.ok());
    manager_ = std::move(result).value();
    ASSERT_NE(manager_, nullptr);
  }

  std::shared_ptr<MockFeatureProvider> mock_provider_;
  std::unique_ptr<FeatureProviderStatusManager> manager_;
  EvaluationContext ctx_;
};

TEST_F(FeatureProviderStatusManagerTest, CreateWithNullProviderReturnsError) {
  auto result = FeatureProviderStatusManager::Create(nullptr);
  ASSERT_FALSE(result.ok());
  EXPECT_EQ(result.status().code(), absl::StatusCode::kInvalidArgument);
}

TEST_F(FeatureProviderStatusManagerTest, CreateWithValidProviderSucceeds) {
  EXPECT_EQ(manager_->GetStatus(), ProviderStatus::kNotReady);
  EXPECT_EQ(manager_->GetProvider(), mock_provider_);
}

class FeatureProviderStatusManagerInitTest
    : public FeatureProviderStatusManagerTest,
      public WithParamInterface<std::tuple<absl::Status, ProviderStatus>> {};

TEST_P(FeatureProviderStatusManagerInitTest, InitSetsCorrectStatus) {
  auto [provider_init_status, expected_manager_status] = GetParam();

  EXPECT_CALL(*mock_provider_, Init(_)).WillOnce(Return(provider_init_status));
  manager_->Init(ctx_);
  EXPECT_EQ(manager_->GetStatus(), expected_manager_status);
}

INSTANTIATE_TEST_SUITE_P(
    InitStatusTests, FeatureProviderStatusManagerInitTest,
    testing::Values(
        std::make_tuple(absl::OkStatus(), ProviderStatus::kReady),
        std::make_tuple(absl::InternalError("Initialization failed"),
                        ProviderStatus::kError)));

class FeatureProviderStatusManagerShutdownTest
    : public FeatureProviderStatusManagerTest,
      public WithParamInterface<absl::Status> {};

TEST_P(FeatureProviderStatusManagerShutdownTest,
       ShutdownAlwaysSetsStatusToNotReady) {
  manager_->SetStatus(ProviderStatus::kReady);
  auto provider_shutdown_status = GetParam();
  EXPECT_CALL(*mock_provider_, Shutdown())
      .WillOnce(Return(provider_shutdown_status));
  manager_->Shutdown();
  EXPECT_EQ(manager_->GetStatus(), ProviderStatus::kNotReady);
}

INSTANTIATE_TEST_SUITE_P(
    ShutdownStatusTests, FeatureProviderStatusManagerShutdownTest,
    testing::Values(absl::OkStatus(), absl::InternalError("Shutdown failed")));

TEST_F(FeatureProviderStatusManagerTest, SetStatusAndGetStatusWorkCorrectly) {
  // Verify initial status from SetUp.
  ASSERT_EQ(manager_->GetStatus(), ProviderStatus::kNotReady);

  // Define a list of statuses to test.
  const std::vector<ProviderStatus> statuses_to_test = {
      ProviderStatus::kStale, ProviderStatus::kFatal, ProviderStatus::kReady,
      ProviderStatus::kNotReady, ProviderStatus::kError};

  // Iterate and test each status.
  for (const auto& status : statuses_to_test) {
    manager_->SetStatus(status);
    EXPECT_EQ(manager_->GetStatus(), status);
  }
}

TEST_F(FeatureProviderStatusManagerTest, GetProviderReturnsCorrectProvider) {
  // The GetProvider method should return the same instance of the provider
  // regardless of the manager's status.
  EXPECT_EQ(manager_->GetProvider(), mock_provider_);
  manager_->SetStatus(ProviderStatus::kReady);
  EXPECT_EQ(manager_->GetProvider(), mock_provider_);
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}