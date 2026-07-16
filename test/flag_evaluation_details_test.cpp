#include "openfeature/flag_evaluation_details.h"

#include <gtest/gtest.h>

#include <map>
#include <optional>
#include <string>

#include "openfeature/error_code.h"
#include "openfeature/flag_metadata.h"
#include "openfeature/reason.h"
#include "openfeature/resolution_details.h"
#include "openfeature/value.h"

namespace openfeature {

TEST(FlagEvaluationDetailsTest, DirectConstructorAccessesFieldsForBoolean) {
  const std::string expected_flag_key = "bool-flag";
  const bool expected_value = true;
  const Reason expected_reason = Reason::kTargetingMatch;
  const std::optional<std::string> expected_variant = "on-variant";
  const FlagMetadata expected_flag_metadata{};
  const std::optional<ErrorCode> expected_error_code = ErrorCode::kParseError;
  const std::optional<std::string> expected_error_message =
      "Failed to parse data";

  BoolFlagEvaluationDetails details(
      expected_flag_key, expected_value, expected_reason, expected_variant,
      expected_flag_metadata, expected_error_code, expected_error_message);

  EXPECT_EQ(details.GetFlagKey(), expected_flag_key);
  EXPECT_EQ(details.GetValue(), expected_value);
  ASSERT_EQ(details.GetReason(), expected_reason);
  ASSERT_EQ(details.GetVariant(), expected_variant);
  ASSERT_EQ(details.GetErrorCode(), expected_error_code);
  ASSERT_EQ(details.GetErrorMessage(), expected_error_message);
  ASSERT_NO_THROW(details.GetFlagMetadata());
}

TEST(FlagEvaluationDetailsTest, ResolutionDetailsConstructorForBoolean) {
  const std::string expected_flag_key = "bool-flag-wrapped";
  const bool expected_value = false;
  const Reason expected_reason = Reason::kDefault;
  const std::optional<std::string> expected_variant = "off-variant";
  const FlagMetadata expected_flag_metadata{};
  const std::optional<ErrorCode> expected_error_code = std::nullopt;
  const std::optional<std::string> expected_error_message = std::nullopt;

  BoolResolutionDetails res_details(
      expected_value, expected_reason, expected_variant, expected_flag_metadata,
      expected_error_code, expected_error_message);

  BoolFlagEvaluationDetails details(expected_flag_key, res_details);

  EXPECT_EQ(details.GetFlagKey(), expected_flag_key);
  EXPECT_EQ(details.GetValue(), expected_value);
  ASSERT_EQ(details.GetReason(), expected_reason);
  ASSERT_EQ(details.GetVariant(), expected_variant);
  ASSERT_EQ(details.GetErrorCode(), expected_error_code);
  ASSERT_EQ(details.GetErrorMessage(), expected_error_message);
  ASSERT_NO_THROW(details.GetFlagMetadata());
}

TEST(FlagEvaluationDetailsTest, DirectConstructorAccessesFieldsForString) {
  const std::string expected_flag_key = "string-flag";
  const std::string expected_value = "expected-string";
  const Reason expected_reason = Reason::kTargetingMatch;
  const std::optional<std::string> expected_variant = "on-variant";
  const FlagMetadata expected_flag_metadata{};
  const std::optional<ErrorCode> expected_error_code = ErrorCode::kTypeMismatch;
  const std::optional<std::string> expected_error_message =
      "Type mismatch error";

  StringFlagEvaluationDetails details(
      expected_flag_key, expected_value, expected_reason, expected_variant,
      expected_flag_metadata, expected_error_code, expected_error_message);

  EXPECT_EQ(details.GetFlagKey(), expected_flag_key);
  EXPECT_EQ(details.GetValue(), expected_value);
  ASSERT_EQ(details.GetReason(), expected_reason);
  ASSERT_EQ(details.GetVariant(), expected_variant);
  ASSERT_EQ(details.GetErrorCode(), expected_error_code);
  ASSERT_EQ(details.GetErrorMessage(), expected_error_message);
  ASSERT_NO_THROW(details.GetFlagMetadata());
}

TEST(FlagEvaluationDetailsTest, ResolutionDetailsConstructorForString) {
  const std::string expected_flag_key = "string-flag-wrapped";
  const std::string expected_value = "wrapped-string";
  const Reason expected_reason = Reason::kStatic;
  const std::optional<std::string> expected_variant = std::nullopt;
  const FlagMetadata expected_flag_metadata{};

  StringResolutionDetails res_details(expected_value, expected_reason,
                                      expected_variant, expected_flag_metadata);

  StringFlagEvaluationDetails details(expected_flag_key, res_details);

  EXPECT_EQ(details.GetFlagKey(), expected_flag_key);
  EXPECT_EQ(details.GetValue(), expected_value);
  ASSERT_EQ(details.GetReason(), expected_reason);
  ASSERT_EQ(details.GetVariant(), expected_variant);
  ASSERT_EQ(details.GetErrorCode(), std::nullopt);
  ASSERT_EQ(details.GetErrorMessage(), std::nullopt);
}

TEST(FlagEvaluationDetailsTest, DirectConstructorAccessesFieldsForInteger) {
  const std::string expected_flag_key = "int-flag";
  const int64_t expected_value = 123456789LL;
  const Reason expected_reason = Reason::kTargetingMatch;
  const std::optional<std::string> expected_variant = "v1";
  const FlagMetadata expected_flag_metadata{};

  IntFlagEvaluationDetails details(expected_flag_key, expected_value,
                                   expected_reason, expected_variant,
                                   expected_flag_metadata);

  EXPECT_EQ(details.GetFlagKey(), expected_flag_key);
  EXPECT_EQ(details.GetValue(), expected_value);
  ASSERT_EQ(details.GetReason(), expected_reason);
  ASSERT_EQ(details.GetVariant(), expected_variant);
  ASSERT_EQ(details.GetErrorCode(), std::nullopt);
  ASSERT_EQ(details.GetErrorMessage(), std::nullopt);
}

TEST(FlagEvaluationDetailsTest, ResolutionDetailsConstructorForInteger) {
  const std::string expected_flag_key = "int-flag-wrapped";
  const int64_t expected_value = 42;
  const Reason expected_reason = Reason::kCached;
  const std::optional<std::string> expected_variant = "v2";
  const FlagMetadata expected_flag_metadata{};

  IntResolutionDetails res_details(expected_value, expected_reason,
                                   expected_variant, expected_flag_metadata);

  IntFlagEvaluationDetails details(expected_flag_key, res_details);

  EXPECT_EQ(details.GetFlagKey(), expected_flag_key);
  EXPECT_EQ(details.GetValue(), expected_value);
  ASSERT_EQ(details.GetReason(), expected_reason);
  ASSERT_EQ(details.GetVariant(), expected_variant);
  ASSERT_EQ(details.GetErrorCode(), std::nullopt);
  ASSERT_EQ(details.GetErrorMessage(), std::nullopt);
}

TEST(FlagEvaluationDetailsTest, DirectConstructorAccessesFieldsForDouble) {
  const std::string expected_flag_key = "double-flag";
  const double expected_value = 123.456;
  const Reason expected_reason = Reason::kTargetingMatch;
  const std::optional<std::string> expected_variant = "on-variant";
  const FlagMetadata expected_flag_metadata{};

  DoubleFlagEvaluationDetails details(expected_flag_key, expected_value,
                                      expected_reason, expected_variant,
                                      expected_flag_metadata);

  EXPECT_EQ(details.GetFlagKey(), expected_flag_key);
  EXPECT_DOUBLE_EQ(details.GetValue(), expected_value);
  ASSERT_EQ(details.GetReason(), expected_reason);
  ASSERT_EQ(details.GetVariant(), expected_variant);
  ASSERT_EQ(details.GetErrorCode(), std::nullopt);
  ASSERT_EQ(details.GetErrorMessage(), std::nullopt);
}

TEST(FlagEvaluationDetailsTest, ResolutionDetailsConstructorForDouble) {
  const std::string expected_flag_key = "double-flag-wrapped";
  const double expected_value = 3.14159265359;
  const Reason expected_reason = Reason::kSplit;
  const std::optional<std::string> expected_variant = "pi-variant";
  const FlagMetadata expected_flag_metadata{};

  DoubleResolutionDetails res_details(expected_value, expected_reason,
                                      expected_variant, expected_flag_metadata);

  DoubleFlagEvaluationDetails details(expected_flag_key, res_details);

  EXPECT_EQ(details.GetFlagKey(), expected_flag_key);
  EXPECT_DOUBLE_EQ(details.GetValue(), expected_value);
  ASSERT_EQ(details.GetReason(), expected_reason);
  ASSERT_EQ(details.GetVariant(), expected_variant);
}

TEST(FlagEvaluationDetailsTest, DirectConstructorAccessesFieldsForObject) {
  const std::string expected_flag_key = "object-flag";
  const Value expected_value = Value(std::map<std::string, Value>{
      {"key1", Value("value1")}, {"key2", Value(42)}});
  const Reason expected_reason = Reason::kTargetingMatch;
  const std::optional<std::string> expected_variant = "object-variant";
  const FlagMetadata expected_flag_metadata{};

  ObjectFlagEvaluationDetails details(expected_flag_key, expected_value,
                                      expected_reason, expected_variant,
                                      expected_flag_metadata);

  EXPECT_EQ(details.GetFlagKey(), expected_flag_key);
  EXPECT_EQ(details.GetValue(), expected_value);
  ASSERT_EQ(details.GetReason(), expected_reason);
  ASSERT_EQ(details.GetVariant(), expected_variant);
  ASSERT_EQ(details.GetErrorCode(), std::nullopt);
  ASSERT_EQ(details.GetErrorMessage(), std::nullopt);
}

TEST(FlagEvaluationDetailsTest, ResolutionDetailsConstructorForObject) {
  const std::string expected_flag_key = "object-flag-wrapped";
  const Value expected_value = Value(std::map<std::string, Value>{
      {"nested", Value(true)}, {"score", Value(99.5)}});
  const Reason expected_reason = Reason::kTargetingMatch;
  const std::optional<std::string> expected_variant = "complex-variant";
  const FlagMetadata expected_flag_metadata{};

  ObjectResolutionDetails res_details(expected_value, expected_reason,
                                      expected_variant, expected_flag_metadata);

  ObjectFlagEvaluationDetails details(expected_flag_key, res_details);

  EXPECT_EQ(details.GetFlagKey(), expected_flag_key);
  EXPECT_EQ(details.GetValue(), expected_value);
  ASSERT_EQ(details.GetReason(), expected_reason);
  ASSERT_EQ(details.GetVariant(), expected_variant);
  ASSERT_EQ(details.GetErrorCode(), std::nullopt);
  ASSERT_EQ(details.GetErrorMessage(), std::nullopt);
}

}  // namespace openfeature
