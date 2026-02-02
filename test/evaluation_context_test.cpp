#include "openfeature/evaluation_context.h"

#include <gtest/gtest.h>

#include <string>
#include <vector>

using namespace openfeature;

class EvaluationContextTest : public ::testing::Test {
 protected:
  // Helper to cast std::any safely for assertions.
  template <typename T>
  T AnyCast(const std::any* value) {
    if (!value) throw std::runtime_error("Value is nullptr");
    return std::any_cast<T>(*value);
  }
};

// Test that a context built with no properties returns expected defaults.
TEST_F(EvaluationContextTest, DefaultBuilderCreatesEmptyContext) {
  EvaluationContext ctx = EvaluationContext::Builder().build();

  // Based on implementation, a missing key in builder becomes "" in
  // constructor.
  auto key = ctx.GetTargetingKey();
  EXPECT_FALSE(key.has_value());

  EXPECT_TRUE(ctx.GetAttributes().empty());
}

// Test setting and retrieving the targeting key.
TEST_F(EvaluationContextTest, BuilderSetsTargetingKey) {
  std::string expected_key = "user-12345";
  EvaluationContext ctx =
      EvaluationContext::Builder().WithTargetingKey(expected_key).build();

  auto key = ctx.GetTargetingKey();
  ASSERT_TRUE(key.has_value());
  EXPECT_EQ(key.value(), expected_key);
}

// Test setting and retrieving various attribute types.
TEST_F(EvaluationContextTest, BuilderSetsAttributesOfVariousTypes) {
  EvaluationContext ctx = EvaluationContext::Builder()
                              .WithAttribute("str_attr", std::string("test"))
                              .WithAttribute("int_attr", 42)
                              .WithAttribute("bool_attr", true)
                              .WithAttribute("double_attr", 3.14)
                              .build();

  const auto& attrs = ctx.GetAttributes();
  EXPECT_EQ(attrs.size(), 4);

  const std::any* str_val = ctx.GetValue("str_attr");
  ASSERT_NE(str_val, nullptr);
  EXPECT_EQ(std::any_cast<std::string>(*str_val), "test");

  const std::any* int_val = ctx.GetValue("int_attr");
  ASSERT_NE(int_val, nullptr);
  EXPECT_EQ(std::any_cast<int>(*int_val), 42);

  const std::any* bool_val = ctx.GetValue("bool_attr");
  ASSERT_NE(bool_val, nullptr);
  EXPECT_TRUE(std::any_cast<bool>(*bool_val));

  const std::any* double_val = ctx.GetValue("double_attr");
  ASSERT_NE(double_val, nullptr);
  EXPECT_DOUBLE_EQ(std::any_cast<double>(*double_val), 3.14);
}

// Test behavior when requesting a non-existent attribute.
TEST_F(EvaluationContextTest, GetValueReturnsNullForMissingKey) {
  EvaluationContext ctx =
      EvaluationContext::Builder().WithAttribute("exists", 1).build();

  EXPECT_NE(ctx.GetValue("exists"), nullptr);
  EXPECT_EQ(ctx.GetValue("does_not_exist"), nullptr);
}

// Test that setting the same attribute key twice overwrites the previous value
// within the same builder chain.
TEST_F(EvaluationContextTest, BuilderOverwritesDuplicateKeys) {
  EvaluationContext ctx = EvaluationContext::Builder()
                              .WithAttribute("key", 100)
                              .WithAttribute("key", 200)
                              .build();

  const std::any* val = ctx.GetValue("key");
  ASSERT_NE(val, nullptr);
  EXPECT_EQ(std::any_cast<int>(*val), 200);
}

// Test merging attributes with precedence.
TEST_F(EvaluationContextTest, MergeAttributesWithPrecedence) {
  EvaluationContext ctx1 = EvaluationContext::Builder()
                               .WithAttribute("common", 1)
                               .WithAttribute("ctx1", std::string("A"))
                               .build();

  EvaluationContext ctx2 = EvaluationContext::Builder()
                               .WithAttribute("common", 2)
                               .WithAttribute("ctx2", std::string("B"))
                               .build();

  EvaluationContext merged = EvaluationContext::Merge({&ctx1, &ctx2});

  EXPECT_EQ(merged.GetAttributes().size(), 3);

  EXPECT_EQ(std::any_cast<int>(*merged.GetValue("common")), 2);
  EXPECT_EQ(std::any_cast<std::string>(*merged.GetValue("ctx1")), "A");
  EXPECT_EQ(std::any_cast<std::string>(*merged.GetValue("ctx2")), "B");
}

// Test that the last context in the list with a valid and non-empty targeting
// key remains as the final result.
TEST_F(EvaluationContextTest, MergeTargetingKeyWithPrecedence) {
  EvaluationContext ctx_no_key = EvaluationContext::Builder().build();
  EvaluationContext ctx_key_a =
      EvaluationContext::Builder().WithTargetingKey("KeyA").build();
  EvaluationContext ctx_key_b =
      EvaluationContext::Builder().WithTargetingKey("KeyB").build();

  EvaluationContext res1 = EvaluationContext::Merge({&ctx_key_a, &ctx_key_b});
  EXPECT_EQ(res1.GetTargetingKey().value(), "KeyB");

  EvaluationContext res2 =
      EvaluationContext::Merge({&ctx_key_a, &ctx_no_key, &ctx_no_key});
  EXPECT_EQ(res2.GetTargetingKey().value(), "KeyA");

  EvaluationContext res3 =
      EvaluationContext::Merge({&ctx_no_key, &ctx_key_b, &ctx_no_key});
  EXPECT_EQ(res3.GetTargetingKey().value(), "KeyB");
}

// Test Merging: Complex scenario with attributes and keys.
TEST_F(EvaluationContextTest, MergeComplexScenario) {
  EvaluationContext base = EvaluationContext::Builder()
                               .WithTargetingKey("base-user")
                               .WithAttribute("env", std::string("prod"))
                               .WithAttribute("region", std::string("us-east"))
                               .build();

  EvaluationContext request =
      EvaluationContext::Builder()
          .WithTargetingKey("req-user")
          .WithAttribute("region", std::string("us-west"))
          .WithAttribute("request_id", 123)
          .build();

  EvaluationContext merged = EvaluationContext::Merge({&base, &request});

  EXPECT_EQ(merged.GetTargetingKey().value(), "req-user");

  EXPECT_EQ(std::any_cast<std::string>(*merged.GetValue("env")), "prod");
  EXPECT_EQ(std::any_cast<std::string>(*merged.GetValue("region")), "us-west");
  EXPECT_EQ(std::any_cast<int>(*merged.GetValue("request_id")), 123);
}

// The merged context should only reflect non-null inputs.
TEST_F(EvaluationContextTest, MergeIgnoresNullPointers) {
  EvaluationContext ctx =
      EvaluationContext::Builder().WithTargetingKey("valid").build();

  EvaluationContext merged = EvaluationContext::Merge({nullptr, &ctx, nullptr});

  EXPECT_EQ(merged.GetTargetingKey().value(), "valid");
}

// Test that string literals and std::string are stored and retrieved correctly.
TEST_F(EvaluationContextTest, StoresStringCorrectly) {
  EvaluationContext ctx_char =
      EvaluationContext::Builder().WithAttribute("k", "v").build();
  const std::any* val_char = ctx_char.GetValue("k");

  // Verify it is stored as std::string, not const char*
  ASSERT_NE(val_char, nullptr);
  EXPECT_EQ(val_char->type(), typeid(std::string));
  EXPECT_EQ(std::any_cast<std::string>(*val_char), "v");

  EvaluationContext ctx_str =
      EvaluationContext::Builder().WithAttribute("k", std::string("v")).build();
  const std::any* val_str = ctx_str.GetValue("k");
  EXPECT_EQ(val_str->type(), typeid(std::string));
  EXPECT_EQ(std::any_cast<std::string>(*val_str), "v");
}
