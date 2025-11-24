#include "openfeature/resolution_details.h"

#include <gtest/gtest.h>

using namespace openfeature;

TEST(ResolutionDetailsTest, AccessesFieldsAfterInitialization) {
  const bool expected_value = true;
  const Reason expected_reason = Reason::kTargetingMatch;
  const std::optional<std::string> expected_variant = "on-variant";
  const std::optional<ErrorCode> expected_error_code = ErrorCode::kParseError;
  const std::optional<std::string> expected_error_message =
      "Failed to parse data";

  const FlagMetadata expected_flag_metadata{};

  BoolResolutionDetails details(expected_value, expected_reason,
                                expected_variant, expected_flag_metadata,
                                expected_error_code, expected_error_message);

  ASSERT_EQ(details.GetValue(), expected_value);
  ASSERT_EQ(details.GetReason(), expected_reason);
  ASSERT_EQ(details.GetVariant(), expected_variant);
  ASSERT_EQ(details.GetErrorCode(), expected_error_code);
  ASSERT_EQ(details.GetErrorMessage(), expected_error_message);
  ASSERT_NO_THROW(details.GetFlagMetadata());
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
