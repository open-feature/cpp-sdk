#include "openfeature/noop_provider.h"

#include <gtest/gtest.h>

using namespace openfeature;

class NoopProviderBooleanTest : public ::testing::Test,
                                public ::testing::WithParamInterface<bool> {};

// Test to verify the metadata returned by the provider
TEST(NoopProviderTest, ShouldReturnProviderNameForMetadata) {
  NoopProvider provider;
  const Metadata metadata = provider.GetMetadata();
  EXPECT_EQ(metadata.name, "Noop Provider");
}

// Test to verify the boolean evaluation returns the default value
TEST_P(NoopProviderBooleanTest, BooleanEvaluationShouldReturnDefaultValue) {
  NoopProvider provider;
  const std::string flagKey = "my-bool-flag";
  const EvaluationContext ctx;
  const bool defaultValue = GetParam();

  const std::unique_ptr<BoolResolutionDetails> details =
      provider.GetBooleanEvaluation(flagKey, defaultValue, ctx);

  EXPECT_EQ(details->GetValue(), defaultValue);
  EXPECT_EQ(details->GetReason(), Reason::kDefault);
  EXPECT_EQ(details->GetVariant(), "default-variant");
  EXPECT_FALSE(details->GetErrorCode().has_value());
  ASSERT_TRUE(details->GetErrorMessage().has_value());
  EXPECT_TRUE(details->GetErrorMessage()->empty());
}

INSTANTIATE_TEST_SUITE_P(BooleanDefaultValues, NoopProviderBooleanTest,
                         ::testing::Values(true, false));

// Main function to initialize and run all tests
int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
