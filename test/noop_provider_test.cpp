#include "openfeature/noop_provider.h"

#include <gtest/gtest.h>

#include "absl/status/status.h"

using ::openfeature::BoolResolutionDetails;
using ::openfeature::DoubleResolutionDetails;
using ::openfeature::EvaluationContext;
using ::openfeature::IntResolutionDetails;
using ::openfeature::Metadata;
using ::openfeature::NoopProvider;
using ::openfeature::ObjectResolutionDetails;
using ::openfeature::Reason;
using ::openfeature::StringResolutionDetails;
using ::openfeature::Value;

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

// Test to verify the boolean evaluation returns the default value.
class NoopProviderBooleanTest : public NoopProviderTest,
                                public ::testing::WithParamInterface<bool> {};

TEST_P(NoopProviderBooleanTest, BooleanEvaluationShouldReturnDefaultValue) {
  const bool default_value = GetParam();

  absl::StatusOr<std::unique_ptr<BoolResolutionDetails>> result =
      provider_.GetBooleanEvaluation("my-bool-flag", default_value, ctx_);

  ASSERT_TRUE(result.ok());
  const std::unique_ptr<BoolResolutionDetails>& details = *result;

  EXPECT_EQ(details->GetValue(), default_value);
  EXPECT_EQ(details->GetReason(), Reason::kDefault);
  EXPECT_EQ(details->GetVariant(), "default-variant");
  EXPECT_FALSE(details->GetErrorCode().has_value());
  ASSERT_TRUE(details->GetErrorMessage().has_value());
  EXPECT_TRUE(details->GetErrorMessage()->empty());
}

INSTANTIATE_TEST_SUITE_P(BooleanDefaultValues, NoopProviderBooleanTest,
                         testing::Values(true, false));

class NoopProviderStringTest
    : public NoopProviderTest,
      public ::testing::WithParamInterface<std::string> {};

// Test to verify the string evaluation returns the default value.
TEST_P(NoopProviderStringTest, StringEvaluationShouldReturnDefaultValue) {
  const std::string& default_value = GetParam();

  absl::StatusOr<std::unique_ptr<StringResolutionDetails>> result =
      provider_.GetStringEvaluation("my-string-flag", default_value, ctx_);
  ASSERT_TRUE(result.ok());
  const std::unique_ptr<StringResolutionDetails>& details = *result;

  EXPECT_EQ(details->GetValue(), default_value);
  EXPECT_EQ(details->GetReason(), Reason::kDefault);
  EXPECT_EQ(details->GetVariant(), "default-variant");
  EXPECT_FALSE(details->GetErrorCode().has_value());
  ASSERT_TRUE(details->GetErrorMessage().has_value());
  EXPECT_TRUE(details->GetErrorMessage()->empty());
}

INSTANTIATE_TEST_SUITE_P(StringDefaultValues, NoopProviderStringTest,
                         testing::Values("hello", "world", ""));

class NoopProviderIntegerTest : public NoopProviderTest,
                                public ::testing::WithParamInterface<int64_t> {
};

// Test to verify the integer evaluation returns the default value.
TEST_P(NoopProviderIntegerTest, IntegerEvaluationShouldReturnDefaultValue) {
  const int64_t default_value = GetParam();

  absl::StatusOr<std::unique_ptr<IntResolutionDetails>> result =
      provider_.GetIntegerEvaluation("my-int-flag", default_value, ctx_);
  ASSERT_TRUE(result.ok());
  const std::unique_ptr<IntResolutionDetails>& details = *result;

  EXPECT_EQ(details->GetValue(), default_value);
  EXPECT_EQ(details->GetReason(), Reason::kDefault);
  EXPECT_EQ(details->GetVariant(), "default-variant");
  EXPECT_FALSE(details->GetErrorCode().has_value());
  ASSERT_TRUE(details->GetErrorMessage().has_value());
  EXPECT_TRUE(details->GetErrorMessage()->empty());
}

INSTANTIATE_TEST_SUITE_P(IntegerDefaultValues, NoopProviderIntegerTest,
                         testing::Values(100, -42, 0, 9223372036854775807LL));

class NoopProviderDoubleTest : public NoopProviderTest,
                               public ::testing::WithParamInterface<double> {};

// Test to verify the double evaluation returns the default value.
TEST_P(NoopProviderDoubleTest, DoubleEvaluationShouldReturnDefaultValue) {
  const double default_value = GetParam();

  absl::StatusOr<std::unique_ptr<DoubleResolutionDetails>> result =
      provider_.GetDoubleEvaluation("my-double-flag", default_value, ctx_);
  ASSERT_TRUE(result.ok());
  const std::unique_ptr<DoubleResolutionDetails>& details = *result;

  EXPECT_DOUBLE_EQ(details->GetValue(), default_value);
  EXPECT_EQ(details->GetReason(), Reason::kDefault);
  EXPECT_EQ(details->GetVariant(), "default-variant");
  EXPECT_FALSE(details->GetErrorCode().has_value());
  ASSERT_TRUE(details->GetErrorMessage().has_value());
  EXPECT_TRUE(details->GetErrorMessage()->empty());
}

constexpr double kPi = 3.14;
constexpr double kNegativeDouble = -100.5;
INSTANTIATE_TEST_SUITE_P(DoubleDefaultValues, NoopProviderDoubleTest,
                         testing::Values(kPi, kNegativeDouble, 0.0));

TEST_F(NoopProviderTest, ObjectEvaluationShouldReturnDefaultValue) {
  constexpr int64_t kDefaultIntValue = 123;
  constexpr double kDefaultListItemValue = 1.23;

  std::map<std::string, Value> inner_struct;
  inner_struct["inner_key"] = Value("inner_value");

  std::map<std::string, Value> default_struct;
  default_struct["a_bool"] = Value(true);
  default_struct["an_int"] = Value(kDefaultIntValue);
  default_struct["a_list"] =
      Value(std::vector<Value>{{Value("item1"), Value(kDefaultListItemValue)}});
  default_struct["a_struct"] = Value(inner_struct);

  const Value default_value(default_struct);

  absl::StatusOr<std::unique_ptr<ObjectResolutionDetails>> result =
      provider_.GetObjectEvaluation("my-object-flag", default_value, ctx_);
  ASSERT_TRUE(result.ok());
  const std::unique_ptr<ObjectResolutionDetails>& details = *result;

  EXPECT_EQ(details->GetValue(), default_value);
  EXPECT_EQ(details->GetReason(), Reason::kDefault);
  EXPECT_EQ(details->GetVariant(), "default-variant");
  EXPECT_FALSE(details->GetErrorCode().has_value());
  ASSERT_TRUE(details->GetErrorMessage().has_value());
  EXPECT_TRUE(details->GetErrorMessage()->empty());
}
