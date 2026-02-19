#include "openfeature/resolution_details.h"

#include <gtest/gtest.h>

#include <optional>

using namespace openfeature;

TEST(ResolutionDetailsTest, AccessesFieldsAfterInitializationForBoolean) {
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

  EXPECT_EQ(details.GetValue(), expected_value);
  ASSERT_EQ(details.GetReason(), expected_reason);
  ASSERT_EQ(details.GetVariant(), expected_variant);
  ASSERT_EQ(details.GetErrorCode(), expected_error_code);
  ASSERT_EQ(details.GetErrorMessage(), expected_error_message);
  ASSERT_NO_THROW(details.GetFlagMetadata());
}

TEST(ResolutionDetailsTest, AccessesFieldsAfterInitializationForString) {
  const std::string expected_value = "expected-string";
  const Reason expected_reason = Reason::kTargetingMatch;
  const std::optional<std::string> expected_variant = "on-variant";
  const std::optional<ErrorCode> expected_error_code = ErrorCode::kParseError;
  const std::optional<std::string> expected_error_message =
      "Failed to parse data";

  const FlagMetadata expected_flag_metadata{};

  StringResolutionDetails details(expected_value, expected_reason,
                                  expected_variant, expected_flag_metadata,
                                  expected_error_code, expected_error_message);

  EXPECT_EQ(details.GetValue(), expected_value);
  ASSERT_EQ(details.GetReason(), expected_reason);
  ASSERT_EQ(details.GetVariant(), expected_variant);
  ASSERT_EQ(details.GetErrorCode(), expected_error_code);
  ASSERT_EQ(details.GetErrorMessage(), expected_error_message);
  ASSERT_NO_THROW(details.GetFlagMetadata());
}

TEST(ResolutionDetailsTest, AccessesFieldsAfterInitializationForInteger) {
  const int64_t expected_value = 123;
  const Reason expected_reason = Reason::kTargetingMatch;
  const std::optional<std::string> expected_variant = "on-variant";
  const std::optional<ErrorCode> expected_error_code = ErrorCode::kParseError;
  const std::optional<std::string> expected_error_message =
      "Failed to parse data";

  const FlagMetadata expected_flag_metadata{};

  IntResolutionDetails details(expected_value, expected_reason,
                               expected_variant, expected_flag_metadata,
                               expected_error_code, expected_error_message);

  EXPECT_EQ(details.GetValue(), expected_value);
  ASSERT_EQ(details.GetReason(), expected_reason);
  ASSERT_EQ(details.GetVariant(), expected_variant);
  ASSERT_EQ(details.GetErrorCode(), expected_error_code);
  ASSERT_EQ(details.GetErrorMessage(), expected_error_message);
  ASSERT_NO_THROW(details.GetFlagMetadata());
}

TEST(ResolutionDetailsTest, AccessesFieldsAfterInitializationForDouble) {
  const double expected_value = 123.456;
  const Reason expected_reason = Reason::kTargetingMatch;
  const std::optional<std::string> expected_variant = "on-variant";
  const std::optional<ErrorCode> expected_error_code = ErrorCode::kParseError;
  const std::optional<std::string> expected_error_message =
      "Failed to parse data";

  const FlagMetadata expected_flag_metadata{};

  DoubleResolutionDetails details(expected_value, expected_reason,
                                  expected_variant, expected_flag_metadata,
                                  expected_error_code, expected_error_message);

  EXPECT_EQ(details.GetValue(), expected_value);
  ASSERT_EQ(details.GetReason(), expected_reason);
  ASSERT_EQ(details.GetVariant(), expected_variant);
  ASSERT_EQ(details.GetErrorCode(), expected_error_code);
  ASSERT_EQ(details.GetErrorMessage(), expected_error_message);
  ASSERT_NO_THROW(details.GetFlagMetadata());
}

TEST(ResolutionDetailsTest, AccessesFieldsAfterInitializationForObject) {
  const Value expected_value = Value(std::map<std::string, Value>{
      {"key1", Value("value1")}, {"key2", Value(42)}});
  const Reason expected_reason = Reason::kTargetingMatch;
  const std::optional<std::string> expected_variant = "on-variant";
  const std::optional<ErrorCode> expected_error_code = ErrorCode::kParseError;
  const std::optional<std::string> expected_error_message =
      "Failed to parse data";

  const FlagMetadata expected_flag_metadata{};

  ObjectResolutionDetails details(expected_value, expected_reason,
                                  expected_variant, expected_flag_metadata,
                                  expected_error_code, expected_error_message);

  EXPECT_EQ(details.GetValue(), expected_value);
  ASSERT_EQ(details.GetReason(), expected_reason);
  ASSERT_EQ(details.GetVariant(), expected_variant);
  ASSERT_EQ(details.GetErrorCode(), expected_error_code);
  ASSERT_EQ(details.GetErrorMessage(), expected_error_message);
  ASSERT_NO_THROW(details.GetFlagMetadata());
}
