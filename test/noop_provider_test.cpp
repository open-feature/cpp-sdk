#include "openfeature/noop_provider.h"

#include <gtest/gtest.h>

using namespace openfeature;

class NoopProviderTest : public ::testing::Test {
 protected:
  NoopProvider provider;
};

// Test to verify the metadata returned by the provider
TEST_F(NoopProviderTest, ShouldReturnProviderNameForMetadata) {
  const Metadata metadata = provider.GetMetadata();
  EXPECT_EQ(metadata.name, "Noop Provider");
}

// Test to verify the boolean evaluation returns the default value
TEST_F(NoopProviderTest, BooleanEvaluationShouldReturnDefaultValue) {
  const std::string flagKey = "my-bool-flag";
  const EvaluationContext ctx;

  // Test case with default value set to true
  {
    const bool defaultValue = true;
    const std::unique_ptr<BoolResolutionDetails> details =
        provider.GetBooleanEvaluation(flagKey, defaultValue, ctx);

    EXPECT_EQ(details->GetValue(), defaultValue);
    EXPECT_EQ(details->GetReason(), Reason::kDefault);
    EXPECT_EQ(details->GetVariant(), "default-variant");
    EXPECT_FALSE(details->GetErrorCode().has_value());
    ASSERT_TRUE(details->GetErrorMessage().has_value());
    EXPECT_TRUE(details->GetErrorMessage()->empty());
  }

  // Test case with default value set to false
  {
    const bool defaultValue = false;
    const std::unique_ptr<BoolResolutionDetails> details =
        provider.GetBooleanEvaluation(flagKey, defaultValue, ctx);

    EXPECT_EQ(details->GetValue(), defaultValue);
    EXPECT_EQ(details->GetReason(), Reason::kDefault);
    EXPECT_EQ(details->GetVariant(), "default-variant");
    EXPECT_FALSE(details->GetErrorCode().has_value());
    ASSERT_TRUE(details->GetErrorMessage().has_value());
    EXPECT_TRUE(details->GetErrorMessage()->empty());
  }
}

// Main function to initialize and run all tests
int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}