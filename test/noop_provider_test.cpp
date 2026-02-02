#include "openfeature/noop_provider.h"

#include <gtest/gtest.h>

#include "absl/status/status.h"

using namespace openfeature;

class NoopProviderTest : public ::testing::Test {
 protected:
  NoopProvider provider_;
  EvaluationContext ctx_ = EvaluationContext::Builder().build();
};

// Test to verify the metadata returned by the provider.
TEST_F(NoopProviderTest, ShouldReturnProviderNameForMetadata) {
  const Metadata metadata = provider_.GetMetadata();
  EXPECT_EQ(metadata.name, "Noop Provider");
}

// Test to verify the Init method returns an OK status.
TEST_F(NoopProviderTest, InitShouldReturnOkStatus) {
  const absl::Status status = provider_.Init(ctx_);
  EXPECT_EQ(status, absl::OkStatus());
}

// Test to verify the Shutdown method returns an OK status.
TEST_F(NoopProviderTest, ShutdownShouldReturnOkStatus) {
  const absl::Status status = provider_.Shutdown();
  EXPECT_EQ(status, absl::OkStatus());
}

class NoopProviderBooleanTest : public NoopProviderTest,
                                public ::testing::WithParamInterface<bool> {};

// Test to verify the boolean evaluation returns the default value.
TEST_P(NoopProviderBooleanTest, BooleanEvaluationShouldReturnDefaultValue) {
  const bool defaultValue = GetParam();

  const std::unique_ptr<BoolResolutionDetails> details =
      provider_.GetBooleanEvaluation("my-bool-flag", defaultValue, ctx_);

  EXPECT_EQ(details->GetValue(), defaultValue);
  EXPECT_EQ(details->GetReason(), Reason::kDefault);
  EXPECT_EQ(details->GetVariant(), "default-variant");
  EXPECT_FALSE(details->GetErrorCode().has_value());
  ASSERT_TRUE(details->GetErrorMessage().has_value());
  EXPECT_TRUE(details->GetErrorMessage()->empty());
}

INSTANTIATE_TEST_SUITE_P(BooleanDefaultValues, NoopProviderBooleanTest,
                         testing::Values(true, false));

// Main function to initialize and run all tests.
int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
