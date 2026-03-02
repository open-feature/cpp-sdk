#include "openfeature/memory_provider/flag.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <string>
#include <unordered_map>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "openfeature/evaluation_context.h"
#include "openfeature/flag_metadata.h"

namespace openfeature {

TEST(FlagTest, InitializesAndReturnsPropertiesCorrectly) {
  std::unordered_map<std::string, std::string> variants = {
      {"v1", std::string("value1")}, {"v2", std::string("value2")}};
  std::string default_variant = "v1";

  Flag<std::string>::ContextEvaluator evaluator =
      [](const Flag<std::string>&,
         const EvaluationContext&) -> absl::StatusOr<std::string> {
    return "evaluated";
  };

  FlagMetadata metadata;
  metadata.data["bool_prop"] = true;
  metadata.data["str_prop"] = std::string("meta_str");
  metadata.data["int_prop"] = int64_t{42};
  metadata.data["double_prop"] = 3.14;

  Flag<std::string> flag(variants, default_variant, evaluator, metadata, true);

  const std::unordered_map<std::string, std::string>& got_variants =
      flag.GetVariants();
  EXPECT_EQ(got_variants.size(), 2);
  EXPECT_EQ(got_variants.at("v1"), "value1");
  EXPECT_EQ(got_variants.at("v2"), "value2");

  EXPECT_EQ(flag.GetDefaultVariant(), "v1");

  const auto& got_evaluator = flag.GetContextEvaluator();
  ASSERT_TRUE(got_evaluator != nullptr);
  absl::StatusOr<std::string> eval_result =
      got_evaluator(flag, EvaluationContext::Builder().build());
  ASSERT_TRUE(eval_result.ok());
  EXPECT_EQ(*eval_result, "evaluated");

  const FlagMetadata& got_metadata = flag.GetFlagMetadata();
  EXPECT_EQ(got_metadata.data.size(), 4);
  EXPECT_EQ(std::get<bool>(got_metadata.data.at("bool_prop")), true);
  EXPECT_EQ(std::get<std::string>(got_metadata.data.at("str_prop")),
            "meta_str");
  EXPECT_EQ(std::get<int64_t>(got_metadata.data.at("int_prop")), 42);
  EXPECT_DOUBLE_EQ(std::get<double>(got_metadata.data.at("double_prop")), 3.14);

  EXPECT_TRUE(flag.IsDisabled());
}

TEST(FlagTest, DefaultDisabledStateIsFalse) {
  Flag<int>::ContextEvaluator evaluator =
      [](const Flag<int>&, const EvaluationContext&) -> absl::StatusOr<int> {
    return 0;
  };

  Flag<int> flag({}, "default", evaluator, FlagMetadata{});
  EXPECT_FALSE(flag.IsDisabled());
}

TEST(FlagTest, EvaluatorUsesTargetingKey) {
  std::unordered_map<std::string, bool> variants = {{"feature_on", true},
                                                    {"feature_off", false}};

  Flag<bool>::ContextEvaluator evaluator =
      [](const Flag<bool>& f,
         const EvaluationContext& ctx) -> absl::StatusOr<bool> {
    std::optional<std::string_view> tk = ctx.GetTargetingKey();
    if (tk && *tk == "beta_tester") {
      return f.GetVariants().at("feature_on");
    }
    return f.GetVariants().at("feature_off");
  };

  Flag<bool> flag(variants, "feature_off", evaluator, FlagMetadata{});
  const auto& eval_func = flag.GetContextEvaluator();

  EvaluationContext beta_ctx =
      EvaluationContext::Builder().WithTargetingKey("beta_tester").build();
  absl::StatusOr<bool> beta_res = eval_func(flag, beta_ctx);
  ASSERT_TRUE(beta_res.ok());
  EXPECT_TRUE(*beta_res);

  EvaluationContext normal_ctx =
      EvaluationContext::Builder().WithTargetingKey("regular_user").build();
  absl::StatusOr<bool> normal_res = eval_func(flag, normal_ctx);
  ASSERT_TRUE(normal_res.ok());
  EXPECT_FALSE(*normal_res);
}

TEST(FlagTest, EvaluatorUsesContextAttributes) {
  std::unordered_map<std::string, int> variants = {{"premium", 100},
                                                   {"standard", 10}};

  Flag<int>::ContextEvaluator evaluator =
      [](const Flag<int>& f,
         const EvaluationContext& ctx) -> absl::StatusOr<int> {
    const std::any* user_tier = ctx.GetValue("tier");

    if (user_tier && user_tier->type() == typeid(std::string)) {
      if (std::any_cast<std::string>(*user_tier) == "premium") {
        return f.GetVariants().at("premium");
      }
    }
    return f.GetVariants().at("standard");
  };

  Flag<int> flag(variants, "standard", evaluator, FlagMetadata{});
  const auto& eval_func = flag.GetContextEvaluator();

  EvaluationContext premium_ctx =
      EvaluationContext::Builder().WithAttribute("tier", "premium").build();
  absl::StatusOr<int> premium_res = eval_func(flag, premium_ctx);
  ASSERT_TRUE(premium_res.ok());
  EXPECT_EQ(*premium_res, 100);

  EvaluationContext standard_ctx =
      EvaluationContext::Builder().WithAttribute("tier", "standard").build();
  absl::StatusOr<int> standard_res = eval_func(flag, standard_ctx);
  ASSERT_TRUE(standard_res.ok());
  EXPECT_EQ(*standard_res, 10);
}

TEST(FlagTest, EvaluatorReturnsErrorStatus) {
  Flag<double>::ContextEvaluator evaluator =
      [](const Flag<double>&,
         const EvaluationContext&) -> absl::StatusOr<double> {
    return absl::InvalidArgumentError("Missing required attribute");
  };

  Flag<double> flag({}, "default", evaluator, FlagMetadata{});

  EvaluationContext empty_ctx = EvaluationContext::Builder().build();
  absl::StatusOr<double> result = flag.GetContextEvaluator()(flag, empty_ctx);

  EXPECT_FALSE(result.ok());
  EXPECT_EQ(result.status().code(), absl::StatusCode::kInvalidArgument);
  EXPECT_EQ(result.status().message(), "Missing required attribute");
}
}  // namespace openfeature