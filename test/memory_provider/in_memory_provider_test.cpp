#include "openfeature/memory_provider/in_memory_provider.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <any>
#include <memory>
#include <string>
#include <unordered_map>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "openfeature/error_code.h"
#include "openfeature/evaluation_context.h"
#include "openfeature/memory_provider/flag.h"
#include "openfeature/reason.h"
#include "openfeature/resolution_details.h"
#include "openfeature/value.h"

namespace openfeature {

using ::testing::Eq;
using ::testing::Optional;

class InMemoryProviderTest : public ::testing::Test {
 protected:
  EvaluationContext empty_ctx_ = EvaluationContext::Builder().build();

  // Helper method to create a generic flag for testing.
  template <typename T>
  Flag<T> CreateFlag(std::unordered_map<std::string, T> variants,
                     std::optional<std::string> default_variant_opt,
                     typename Flag<T>::ContextEvaluator evaluator = nullptr,
                     bool disabled = false) {
    return Flag<T>(std::move(variants), std::move(default_variant_opt),
                   std::move(evaluator), FlagMetadata{}, disabled);
  }
};

TEST_F(InMemoryProviderTest, GetMetadataReturnsCorrectName) {
  InMemoryProvider provider({});
  EXPECT_EQ(provider.GetMetadata().name, "InMemoryProvider");
}

TEST_F(InMemoryProviderTest, EvaluationFailsWhenNotReady) {
  InMemoryProvider provider({});

  // Evaluating without calling Init().
  absl::StatusOr<std::unique_ptr<BoolResolutionDetails>> res_or =
      provider.GetBooleanEvaluation("any_flag", false, empty_ctx_);
  ASSERT_TRUE(res_or.ok());
  const std::unique_ptr<BoolResolutionDetails>& res = *res_or;

  ASSERT_NE(res, nullptr);
  EXPECT_EQ(res->GetReason(), Reason::kError);
  EXPECT_EQ(res->GetErrorCode(), ErrorCode::kProviderNotReady);
  EXPECT_FALSE(res->GetValue());
}

TEST_F(InMemoryProviderTest, InitAndShutdownUpdateStateCorrectly) {
  InMemoryProvider provider({});

  EXPECT_TRUE(provider.Init(empty_ctx_).ok());
  absl::StatusOr<std::unique_ptr<BoolResolutionDetails>> res_or =
      provider.GetBooleanEvaluation("any_flag", false, empty_ctx_);
  ASSERT_TRUE(res_or.ok());
  const std::unique_ptr<BoolResolutionDetails>& res = *res_or;

  // After initialization, the state is Ready, but flag is missing.
  EXPECT_EQ(res->GetErrorCode(), ErrorCode::kFlagNotFound);

  EXPECT_TRUE(provider.Shutdown().ok());
  absl::StatusOr<std::unique_ptr<BoolResolutionDetails>> res_or2 =
      provider.GetBooleanEvaluation("any_flag", false, empty_ctx_);
  ASSERT_TRUE(res_or2.ok());
  const std::unique_ptr<BoolResolutionDetails>& res2 = *res_or2;

  EXPECT_EQ(res2->GetErrorCode(), ErrorCode::kProviderNotReady);
}

TEST_F(InMemoryProviderTest, FlagNotFound) {
  InMemoryProvider provider({});
  EXPECT_TRUE(provider.Init(empty_ctx_).ok());

  absl::StatusOr<std::unique_ptr<BoolResolutionDetails>> res_or =
      provider.GetBooleanEvaluation("missing", true, empty_ctx_);
  ASSERT_TRUE(res_or.ok());
  const std::unique_ptr<BoolResolutionDetails>& res = *res_or;

  EXPECT_TRUE(res->GetValue());
  EXPECT_EQ(res->GetReason(), Reason::kError);
  EXPECT_EQ(res->GetErrorCode(), ErrorCode::kFlagNotFound);
}

TEST_F(InMemoryProviderTest, FlagTypeMismatch) {
  std::unordered_map<std::string, std::string> variants = {
      {"v1", std::string("test")}};
  Flag<std::string> str_flag(variants, "v1", nullptr, FlagMetadata{});

  InMemoryProvider provider({});
  provider.UpdateFlag("str_flag", str_flag);
  EXPECT_TRUE(provider.Init(empty_ctx_).ok());

  absl::StatusOr<std::unique_ptr<BoolResolutionDetails>> res_or =
      provider.GetBooleanEvaluation("str_flag", false, empty_ctx_);
  ASSERT_TRUE(res_or.ok());
  const std::unique_ptr<BoolResolutionDetails>& res = *res_or;

  EXPECT_FALSE(res->GetValue());
  EXPECT_EQ(res->GetReason(), Reason::kError);
  EXPECT_EQ(res->GetErrorCode(), ErrorCode::kTypeMismatch);
}

TEST_F(InMemoryProviderTest, DisabledFlagReturnsDisabledReason) {
  InMemoryProvider provider({});
  provider.UpdateFlag("disabled_flag",
                      CreateFlag<bool>({{"on", true}}, "on", nullptr, true));
  EXPECT_TRUE(provider.Init(empty_ctx_).ok());

  absl::StatusOr<std::unique_ptr<BoolResolutionDetails>> res_or =
      provider.GetBooleanEvaluation("disabled_flag", false, empty_ctx_);
  ASSERT_TRUE(res_or.ok());
  const std::unique_ptr<BoolResolutionDetails>& res = *res_or;

  EXPECT_FALSE(res->GetValue());  // fallback to default param.
  EXPECT_EQ(res->GetReason(), Reason::kDisabled);
  EXPECT_FALSE(res->GetErrorCode().has_value());
}

TEST_F(InMemoryProviderTest, StaticEvaluationSuccess) {
  InMemoryProvider provider({});
  provider.UpdateFlag("static_flag",
                      CreateFlag<bool>({{"on", true}, {"off", false}}, "on"));
  EXPECT_TRUE(provider.Init(empty_ctx_).ok());

  absl::StatusOr<std::unique_ptr<BoolResolutionDetails>> res_or =
      provider.GetBooleanEvaluation("static_flag", false, empty_ctx_);
  ASSERT_TRUE(res_or.ok());
  const std::unique_ptr<BoolResolutionDetails>& res = *res_or;

  EXPECT_TRUE(res->GetValue());
  EXPECT_EQ(res->GetReason(), Reason::kStatic);
  EXPECT_THAT(res->GetVariant(), Optional(std::string("on")));
}

TEST_F(InMemoryProviderTest, ContextEvaluatorSuccess) {
  auto evaluator = [](const Flag<bool>&,
                      const EvaluationContext&) -> absl::StatusOr<bool> {
    return false;
  };

  InMemoryProvider provider({});
  provider.UpdateFlag(
      "dyn_flag",
      CreateFlag<bool>({{"on", true}, {"off", false}}, "on", evaluator));
  EXPECT_TRUE(provider.Init(empty_ctx_).ok());

  absl::StatusOr<std::unique_ptr<BoolResolutionDetails>> res_or =
      provider.GetBooleanEvaluation("dyn_flag", true, empty_ctx_);
  ASSERT_TRUE(res_or.ok());
  const std::unique_ptr<BoolResolutionDetails>& res = *res_or;

  EXPECT_FALSE(res->GetValue());
  EXPECT_EQ(res->GetReason(), Reason::kTargetingMatch);
}

TEST_F(InMemoryProviderTest, ContextEvaluatorFailureFallsBackToDefaultVariant) {
  auto evaluator = [](const Flag<bool>&,
                      const EvaluationContext&) -> absl::StatusOr<bool> {
    return absl::InvalidArgumentError("Evaluator failed");
  };

  InMemoryProvider provider({});
  provider.UpdateFlag(
      "dyn_flag",
      CreateFlag<bool>({{"on", true}, {"off", false}}, "off", evaluator));
  EXPECT_TRUE(provider.Init(empty_ctx_).ok());

  absl::StatusOr<std::unique_ptr<BoolResolutionDetails>> res_or =
      provider.GetBooleanEvaluation("dyn_flag", true, empty_ctx_);
  ASSERT_TRUE(res_or.ok());
  const std::unique_ptr<BoolResolutionDetails>& res = *res_or;

  EXPECT_FALSE(res->GetValue());
  EXPECT_EQ(res->GetReason(), Reason::kDefault);
  EXPECT_THAT(res->GetVariant(), Optional(std::string("off")));
}

TEST_F(InMemoryProviderTest, FallbackFailsIfVariantTypeMismatch) {
  std::unordered_map<std::string, bool> variants = {{"v1", true}};
  Flag<bool> bad_flag(variants, "v2", nullptr, FlagMetadata{});

  InMemoryProvider provider({});
  provider.UpdateFlag("missing_variant_flag", bad_flag);
  EXPECT_TRUE(provider.Init(empty_ctx_).ok());

  absl::StatusOr<std::unique_ptr<BoolResolutionDetails>> res_or =
      provider.GetBooleanEvaluation("missing_variant_flag", true, empty_ctx_);
  ASSERT_TRUE(res_or.ok());
  const std::unique_ptr<BoolResolutionDetails>& res = *res_or;

  EXPECT_TRUE(res->GetValue());
  EXPECT_EQ(res->GetReason(), Reason::kError);
  EXPECT_EQ(res->GetErrorCode(), ErrorCode::kParseError);
}

TEST_F(InMemoryProviderTest, FallbackFailsIfVariantMissing) {
  std::unordered_map<std::string, bool> variants = {{"v1", true}};
  Flag<bool> bad_flag(variants, "v2", nullptr, FlagMetadata{});

  InMemoryProvider provider({});
  provider.UpdateFlag("missing_variant_flag", bad_flag);
  EXPECT_TRUE(provider.Init(empty_ctx_).ok());

  absl::StatusOr<std::unique_ptr<BoolResolutionDetails>> res_or =
      provider.GetBooleanEvaluation("missing_variant_flag", true, empty_ctx_);
  ASSERT_TRUE(res_or.ok());
  const std::unique_ptr<BoolResolutionDetails>& res = *res_or;

  EXPECT_TRUE(res->GetValue());
  EXPECT_EQ(res->GetReason(), Reason::kError);
  EXPECT_EQ(res->GetErrorCode(), ErrorCode::kParseError);
  EXPECT_THAT(
      res->GetErrorMessage(),
      Optional(std::string("Default variant v2 not found in variants map")));
}

TEST_F(InMemoryProviderTest, UpdateFlagReplacesAndAddsNew) {
  InMemoryProvider provider({});
  EXPECT_TRUE(provider.Init(empty_ctx_).ok());

  provider.UpdateFlag("flag1", CreateFlag<bool>({{"on", true}}, "on"));
  absl::StatusOr<std::unique_ptr<BoolResolutionDetails>> res1 =
      provider.GetBooleanEvaluation("flag1", false, empty_ctx_);
  ASSERT_TRUE(res1.ok());
  EXPECT_TRUE((*res1)->GetValue());

  provider.UpdateFlag("flag1", CreateFlag<bool>({{"off", false}}, "off"));
  absl::StatusOr<std::unique_ptr<BoolResolutionDetails>> res2 =
      provider.GetBooleanEvaluation("flag1", true, empty_ctx_);
  ASSERT_TRUE(res2.ok());
  EXPECT_FALSE((*res2)->GetValue());

  provider.UpdateFlag("new_flag", CreateFlag<bool>({{"added", true}}, "added"));
  absl::StatusOr<std::unique_ptr<BoolResolutionDetails>> res3 =
      provider.GetBooleanEvaluation("new_flag", false, empty_ctx_);
  ASSERT_TRUE(res3.ok());
  EXPECT_TRUE((*res3)->GetValue());
}

TEST_F(InMemoryProviderTest, UpdateFlagsAddsAndOverwritesExisting) {
  std::unordered_map<std::string, std::any> initial;
  initial["flag1"] = CreateFlag<bool>({{"on", true}}, "on");
  initial["common_flag"] = CreateFlag<bool>({{"initial", false}}, "initial");

  InMemoryProvider provider(std::move(initial));
  EXPECT_TRUE(provider.Init(empty_ctx_).ok());
  absl::StatusOr<std::unique_ptr<BoolResolutionDetails>> res1 =
      provider.GetBooleanEvaluation("flag1", false, empty_ctx_);
  ASSERT_TRUE(res1.ok());
  EXPECT_TRUE((*res1)->GetValue());

  absl::StatusOr<std::unique_ptr<BoolResolutionDetails>> res2 =
      provider.GetBooleanEvaluation("common_flag", true, empty_ctx_);
  ASSERT_TRUE(res2.ok());
  EXPECT_FALSE((*res2)->GetValue());

  std::unordered_map<std::string, std::any> updated;
  updated["flag2"] = CreateFlag<bool>({{"off", false}}, "off");
  updated["common_flag"] = CreateFlag<bool>({{"updated", true}}, "updated");

  provider.UpdateFlags(std::move(updated));

  // flag1 should still exist and retain its value as it was not in
  // `updated_flags_map`
  absl::StatusOr<std::unique_ptr<BoolResolutionDetails>> flag1_res_or =
      provider.GetBooleanEvaluation("flag1", false, empty_ctx_);
  ASSERT_TRUE(flag1_res_or.ok());
  const std::unique_ptr<BoolResolutionDetails>& flag1_res = *flag1_res_or;
  EXPECT_TRUE(flag1_res->GetValue());
  EXPECT_EQ(flag1_res->GetReason(), Reason::kStatic);
  EXPECT_THAT(flag1_res->GetVariant(), Optional(std::string("on")));

  // flag2 should now exist and be evaluated to its new value
  absl::StatusOr<std::unique_ptr<BoolResolutionDetails>> flag2_res_or =
      provider.GetBooleanEvaluation("flag2", true, empty_ctx_);
  ASSERT_TRUE(flag2_res_or.ok());
  const std::unique_ptr<BoolResolutionDetails>& flag2_res = *flag2_res_or;
  EXPECT_FALSE(flag2_res->GetValue());
  EXPECT_EQ(flag2_res->GetReason(), Reason::kStatic);
  EXPECT_THAT(flag2_res->GetVariant(), Optional(std::string("off")));

  // common_flag should be updated to true
  absl::StatusOr<std::unique_ptr<BoolResolutionDetails>> common_flag_res_or =
      provider.GetBooleanEvaluation("common_flag", false, empty_ctx_);
  ASSERT_TRUE(common_flag_res_or.ok());
  const std::unique_ptr<BoolResolutionDetails>& common_flag_res =
      *common_flag_res_or;
  EXPECT_TRUE(common_flag_res->GetValue());
  EXPECT_EQ(common_flag_res->GetReason(), Reason::kStatic);
  EXPECT_THAT(common_flag_res->GetVariant(), Optional(std::string("updated")));
}

TEST_F(InMemoryProviderTest, NoDefaultVariantAndEvaluatorFailsOrMissing) {
  InMemoryProvider provider1({});
  provider1.UpdateFlag("no_default_no_evaluator",
                       CreateFlag<bool>({{"v1", true}}, std::nullopt, nullptr));
  EXPECT_TRUE(provider1.Init(empty_ctx_).ok());
  absl::StatusOr<std::unique_ptr<BoolResolutionDetails>> res1_or =
      provider1.GetBooleanEvaluation("no_default_no_evaluator", true,
                                     empty_ctx_);
  ASSERT_TRUE(res1_or.ok());
  const std::unique_ptr<BoolResolutionDetails>& res1 = *res1_or;
  EXPECT_TRUE(res1->GetValue());
  EXPECT_EQ(res1->GetReason(), Reason::kDefault);
  EXPECT_FALSE(res1->GetVariant().has_value());

  auto failing_evaluator =
      [](const Flag<bool>&, const EvaluationContext&) -> absl::StatusOr<bool> {
    return absl::InvalidArgumentError("Evaluator explicitly failed");
  };
  InMemoryProvider provider2({});
  provider2.UpdateFlag(
      "no_default_failing_evaluator",
      CreateFlag<bool>({{"v1", true}}, std::nullopt, failing_evaluator));
  EXPECT_TRUE(provider2.Init(empty_ctx_).ok());
  absl::StatusOr<std::unique_ptr<BoolResolutionDetails>> res2_or =
      provider2.GetBooleanEvaluation("no_default_failing_evaluator", false,
                                     empty_ctx_);
  ASSERT_TRUE(res2_or.ok());
  const std::unique_ptr<BoolResolutionDetails>& res2 = *res2_or;
  EXPECT_FALSE(res2->GetValue());
  EXPECT_EQ(res2->GetReason(), Reason::kDefault);
  EXPECT_FALSE(res2->GetVariant().has_value());
}

TEST_F(InMemoryProviderTest, ContextEvaluatorUsesContext) {
  auto context_aware_evaluator =
      [](const Flag<bool>&,
         const EvaluationContext& ctx) -> absl::StatusOr<bool> {
    const std::any* val_ptr = ctx.GetValue("user_is_admin");
    if (val_ptr != nullptr) {
      try {
        return std::any_cast<bool>(*val_ptr);
      } catch (const std::bad_any_cast& e) {
        return absl::InvalidArgumentError(
            "Context attribute 'user_is_admin' is not of type bool");
      }
    }
    return false;
  };

  InMemoryProvider provider({});
  provider.UpdateFlag("admin_flag",
                      CreateFlag<bool>({{"on", true}, {"off", false}}, "off",
                                       context_aware_evaluator));
  EXPECT_TRUE(provider.Init(empty_ctx_).ok());

  EvaluationContext admin_ctx =
      EvaluationContext::Builder().WithAttribute("user_is_admin", true).build();
  absl::StatusOr<std::unique_ptr<BoolResolutionDetails>> res_admin_or =
      provider.GetBooleanEvaluation("admin_flag", false, admin_ctx);
  ASSERT_TRUE(res_admin_or.ok());
  const std::unique_ptr<BoolResolutionDetails>& res_admin = *res_admin_or;
  ASSERT_NE(res_admin, nullptr);
  EXPECT_TRUE(res_admin->GetValue());
  EXPECT_EQ(res_admin->GetReason(), Reason::kTargetingMatch);
  EXPECT_FALSE(res_admin->GetErrorCode().has_value());

  EvaluationContext non_admin_ctx = EvaluationContext::Builder()
                                        .WithAttribute("user_is_admin", false)
                                        .build();
  absl::StatusOr<std::unique_ptr<BoolResolutionDetails>> res_non_admin_or =
      provider.GetBooleanEvaluation("admin_flag", true, non_admin_ctx);
  ASSERT_TRUE(res_non_admin_or.ok());
  const std::unique_ptr<BoolResolutionDetails>& res_non_admin =
      *res_non_admin_or;
  ASSERT_NE(res_non_admin, nullptr);
  EXPECT_FALSE(res_non_admin->GetValue());
  EXPECT_EQ(res_non_admin->GetReason(), Reason::kTargetingMatch);
  EXPECT_FALSE(res_non_admin->GetErrorCode().has_value());

  absl::StatusOr<std::unique_ptr<BoolResolutionDetails>> res_no_attr_or =
      provider.GetBooleanEvaluation("admin_flag", true, empty_ctx_);
  ASSERT_TRUE(res_no_attr_or.ok());
  const std::unique_ptr<BoolResolutionDetails>& res_no_attr = *res_no_attr_or;
  ASSERT_NE(res_no_attr, nullptr);
  EXPECT_FALSE(res_no_attr->GetValue());
  EXPECT_EQ(res_no_attr->GetReason(), Reason::kTargetingMatch);
  EXPECT_FALSE(res_no_attr->GetErrorCode().has_value());

  EvaluationContext wrong_type_ctx =
      EvaluationContext::Builder()
          .WithAttribute("user_is_admin", std::string("true"))
          .build();
  absl::StatusOr<std::unique_ptr<BoolResolutionDetails>> res_wrong_type_or =
      provider.GetBooleanEvaluation("admin_flag", true, wrong_type_ctx);
  ASSERT_TRUE(res_wrong_type_or.ok());
  const std::unique_ptr<BoolResolutionDetails>& res_wrong_type =
      *res_wrong_type_or;
  ASSERT_NE(res_wrong_type, nullptr);
  EXPECT_FALSE(res_wrong_type->GetValue());
  EXPECT_EQ(res_wrong_type->GetReason(), Reason::kDefault);
  EXPECT_THAT(res_wrong_type->GetVariant(), Optional(std::string("off")));
  EXPECT_FALSE(res_wrong_type->GetErrorCode().has_value());
}

TEST_F(InMemoryProviderTest, StringEvaluationSuccess) {
  InMemoryProvider provider({});
  provider.UpdateFlag(
      "string_flag",
      CreateFlag<std::string>({{"v1", "hello"}, {"v2", "world"}}, "v2"));
  EXPECT_TRUE(provider.Init(empty_ctx_).ok());

  absl::StatusOr<std::unique_ptr<StringResolutionDetails>> res_or =
      provider.GetStringEvaluation("string_flag", "default", empty_ctx_);
  ASSERT_TRUE(res_or.ok());
  const std::unique_ptr<StringResolutionDetails>& res = *res_or;
  ASSERT_NE(res, nullptr);
  EXPECT_EQ(res->GetValue(), "world");
  EXPECT_EQ(res->GetReason(), Reason::kStatic);
  EXPECT_THAT(res->GetVariant(), Optional(std::string("v2")));
}

TEST_F(InMemoryProviderTest, IntegerEvaluationSuccess) {
  InMemoryProvider provider({});
  provider.UpdateFlag("int_flag",
                      CreateFlag<int64_t>({{"v1", 100}, {"v2", 200}}, "v1"));
  EXPECT_TRUE(provider.Init(empty_ctx_).ok());

  absl::StatusOr<std::unique_ptr<IntResolutionDetails>> res_or =
      provider.GetIntegerEvaluation("int_flag", 0, empty_ctx_);
  ASSERT_TRUE(res_or.ok());
  const std::unique_ptr<IntResolutionDetails>& res = *res_or;

  ASSERT_NE(res, nullptr);
  EXPECT_EQ(res->GetValue(), 100);
  EXPECT_EQ(res->GetReason(), Reason::kStatic);
  EXPECT_THAT(res->GetVariant(), Optional(std::string("v1")));
}

TEST_F(InMemoryProviderTest, DoubleEvaluationSuccess) {
  InMemoryProvider provider({});
  provider.UpdateFlag("double_flag",
                      CreateFlag<double>({{"v1", 3.14}, {"v2", 2.71}}, "v2"));
  EXPECT_TRUE(provider.Init(empty_ctx_).ok());

  absl::StatusOr<std::unique_ptr<DoubleResolutionDetails>> res_or =
      provider.GetDoubleEvaluation("double_flag", 0.0, empty_ctx_);
  ASSERT_TRUE(res_or.ok());
  const std::unique_ptr<DoubleResolutionDetails>& res = *res_or;

  ASSERT_NE(res, nullptr);
  EXPECT_DOUBLE_EQ(res->GetValue(), 2.71);
  EXPECT_EQ(res->GetReason(), Reason::kStatic);
  EXPECT_THAT(res->GetVariant(), Optional(std::string("v2")));
}

TEST_F(InMemoryProviderTest, ObjectEvaluationSuccess) {
  InMemoryProvider provider({});
  provider.UpdateFlag("object_flag",
                      CreateFlag<Value>({{"v1", Value()}}, "v1"));
  EXPECT_TRUE(provider.Init(empty_ctx_).ok());

  absl::StatusOr<std::unique_ptr<ObjectResolutionDetails>> res_or =
      provider.GetObjectEvaluation("object_flag", Value(), empty_ctx_);
  ASSERT_TRUE(res_or.ok());
  const std::unique_ptr<ObjectResolutionDetails>& res = *res_or;

  ASSERT_NE(res, nullptr);
  EXPECT_EQ(res->GetReason(), Reason::kStatic);
  EXPECT_THAT(res->GetVariant(), Optional(std::string("v1")));
}

}  // namespace openfeature
