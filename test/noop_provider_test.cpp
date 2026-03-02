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

// Test to verify the boolean evaluation returns the default value.
class NoopProviderBooleanTest : public NoopProviderTest,
                                public ::testing::WithParamInterface<bool> {};

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

class NoopProviderStringTest
    : public NoopProviderTest,
      public ::testing::WithParamInterface<std::string> {};

// Test to verify the string evaluation returns the default value.
TEST_P(NoopProviderStringTest, StringEvaluationShouldReturnDefaultValue) {
  const std::string defaultValue = GetParam();

  const std::unique_ptr<StringResolutionDetails> details =
      provider_.GetStringEvaluation("my-string-flag", defaultValue, ctx_);

  EXPECT_EQ(details->GetValue(), defaultValue);
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
  const int64_t defaultValue = GetParam();

  const std::unique_ptr<IntResolutionDetails> details =
      provider_.GetIntegerEvaluation("my-int-flag", defaultValue, ctx_);

  EXPECT_EQ(details->GetValue(), defaultValue);
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
  const double defaultValue = GetParam();

  const std::unique_ptr<DoubleResolutionDetails> details =
      provider_.GetDoubleEvaluation("my-double-flag", defaultValue, ctx_);

  EXPECT_DOUBLE_EQ(details->GetValue(), defaultValue);
  EXPECT_EQ(details->GetReason(), Reason::kDefault);
  EXPECT_EQ(details->GetVariant(), "default-variant");
  EXPECT_FALSE(details->GetErrorCode().has_value());
  ASSERT_TRUE(details->GetErrorMessage().has_value());
  EXPECT_TRUE(details->GetErrorMessage()->empty());
}

INSTANTIATE_TEST_SUITE_P(DoubleDefaultValues, NoopProviderDoubleTest,
                         testing::Values(3.14, -100.5, 0.0));

TEST_F(NoopProviderTest, ObjectEvaluationShouldReturnDefaultValue) {
  std::map<std::string, Value> inner_struct;
  inner_struct["inner_key"] = Value("inner_value");

  std::map<std::string, Value> default_struct;
  default_struct["a_bool"] = Value(true);
  default_struct["an_int"] = Value(123);
  default_struct["a_list"] =
      Value(std::vector<Value>{{Value("item1"), Value(1.23)}});
  default_struct["a_struct"] = Value(inner_struct);

  const Value defaultValue(default_struct);

  const std::unique_ptr<ObjectResolutionDetails> details =
      provider_.GetObjectEvaluation("my-object-flag", defaultValue, ctx_);

  EXPECT_EQ(details->GetValue(), defaultValue);
  EXPECT_EQ(details->GetReason(), Reason::kDefault);
  EXPECT_EQ(details->GetVariant(), "default-variant");
  EXPECT_FALSE(details->GetErrorCode().has_value());
  ASSERT_TRUE(details->GetErrorMessage().has_value());
  EXPECT_TRUE(details->GetErrorMessage()->empty());
}
