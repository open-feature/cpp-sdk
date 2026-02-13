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

namespace openfeature {

using ::testing::Eq;
using ::testing::Optional;

class InMemoryProviderTest : public ::testing::Test {
 protected:
  EvaluationContext empty_ctx_ = EvaluationContext::Builder().build();

  // Helper method to create a basic boolean flag for testing.
  Flag<bool> CreateBoolFlag(std::unordered_map<std::string, bool> variants,
                            std::optional<std::string> default_variant_opt,
                            Flag<bool>::ContextEvaluator evaluator = nullptr,
                            bool disabled = false) {
    return Flag<bool>(std::move(variants), std::move(default_variant_opt),
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
  std::unique_ptr<BoolResolutionDetails> res =
      provider.GetBooleanEvaluation("any_flag", false, empty_ctx_);

  ASSERT_NE(res, nullptr);
  EXPECT_EQ(res->GetReason(), Reason::kError);
  EXPECT_EQ(res->GetErrorCode(), ErrorCode::kProviderNotReady);
  EXPECT_FALSE(res->GetValue());
}

TEST_F(InMemoryProviderTest, InitAndShutdownUpdateStateCorrectly) {
  InMemoryProvider provider({});

  EXPECT_TRUE(provider.Init(empty_ctx_).ok());
  std::unique_ptr<BoolResolutionDetails> res =
      provider.GetBooleanEvaluation("any_flag", false, empty_ctx_);

  // After initialization, the state is Ready, but flag is missing.
  EXPECT_EQ(res->GetErrorCode(), ErrorCode::kFlagNotFound);

  EXPECT_TRUE(provider.Shutdown().ok());
  res = provider.GetBooleanEvaluation("any_flag", false, empty_ctx_);

  EXPECT_EQ(res->GetErrorCode(), ErrorCode::kProviderNotReady);
}

TEST_F(InMemoryProviderTest, FlagNotFound) {
  InMemoryProvider provider({});
  EXPECT_TRUE(provider.Init(empty_ctx_).ok());

  std::unique_ptr<BoolResolutionDetails> res =
      provider.GetBooleanEvaluation("missing", true, empty_ctx_);

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

  std::unique_ptr<BoolResolutionDetails> res =
      provider.GetBooleanEvaluation("str_flag", false, empty_ctx_);

  EXPECT_FALSE(res->GetValue());
  EXPECT_EQ(res->GetReason(), Reason::kError);
  EXPECT_EQ(res->GetErrorCode(), ErrorCode::kTypeMismatch);
}

TEST_F(InMemoryProviderTest, DisabledFlagReturnsDisabledReason) {
  InMemoryProvider provider({});
  provider.UpdateFlag("disabled_flag",
                      CreateBoolFlag({{"on", true}}, "on", nullptr, true));
  EXPECT_TRUE(provider.Init(empty_ctx_).ok());

  std::unique_ptr<BoolResolutionDetails> res =
      provider.GetBooleanEvaluation("disabled_flag", false, empty_ctx_);

  EXPECT_FALSE(res->GetValue());  // fallback to default param.
  EXPECT_EQ(res->GetReason(), Reason::kDisabled);
  EXPECT_FALSE(res->GetErrorCode().has_value());
}

TEST_F(InMemoryProviderTest, StaticEvaluationSuccess) {
  InMemoryProvider provider({});
  provider.UpdateFlag("static_flag",
                      CreateBoolFlag({{"on", true}, {"off", false}}, "on"));
  EXPECT_TRUE(provider.Init(empty_ctx_).ok());

  std::unique_ptr<BoolResolutionDetails> res =
      provider.GetBooleanEvaluation("static_flag", false, empty_ctx_);

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
  provider.UpdateFlag("dyn_flag", CreateBoolFlag({{"on", true}, {"off", false}},
                                                 "on", evaluator));
  EXPECT_TRUE(provider.Init(empty_ctx_).ok());

  std::unique_ptr<BoolResolutionDetails> res =
      provider.GetBooleanEvaluation("dyn_flag", true, empty_ctx_);

  EXPECT_FALSE(res->GetValue());
  EXPECT_EQ(res->GetReason(), Reason::kTargetingMatch);
}

TEST_F(InMemoryProviderTest, ContextEvaluatorFailureFallsBackToDefaultVariant) {
  auto evaluator = [](const Flag<bool>&,
                      const EvaluationContext&) -> absl::StatusOr<bool> {
    return absl::InvalidArgumentError("Evaluator failed");
  };

  InMemoryProvider provider({});
  provider.UpdateFlag("dyn_flag", CreateBoolFlag({{"on", true}, {"off", false}},
                                                 "off", evaluator));
  EXPECT_TRUE(provider.Init(empty_ctx_).ok());

  std::unique_ptr<BoolResolutionDetails> res =
      provider.GetBooleanEvaluation("dyn_flag", true, empty_ctx_);

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

  std::unique_ptr<BoolResolutionDetails> res =
      provider.GetBooleanEvaluation("missing_variant_flag", true, empty_ctx_);
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

  std::unique_ptr<BoolResolutionDetails> res =
      provider.GetBooleanEvaluation("missing_variant_flag", true, empty_ctx_);

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

  provider.UpdateFlag("flag1", CreateBoolFlag({{"on", true}}, "on"));
  EXPECT_TRUE(
      provider.GetBooleanEvaluation("flag1", false, empty_ctx_)->GetValue());

  provider.UpdateFlag("flag1", CreateBoolFlag({{"off", false}}, "off"));
  EXPECT_FALSE(
      provider.GetBooleanEvaluation("flag1", true, empty_ctx_)->GetValue());

  provider.UpdateFlag("new_flag", CreateBoolFlag({{"added", true}}, "added"));
  EXPECT_TRUE(
      provider.GetBooleanEvaluation("new_flag", false, empty_ctx_)->GetValue());
}

TEST_F(InMemoryProviderTest, UpdateFlagsAddsAndOverwritesExisting) {
  std::unordered_map<std::string, std::any> initial;
  initial["flag1"] = CreateBoolFlag({{"on", true}}, "on");
  initial["common_flag"] = CreateBoolFlag({{"initial", false}}, "initial");

  InMemoryProvider provider(std::move(initial));
  EXPECT_TRUE(provider.Init(empty_ctx_).ok());
  EXPECT_TRUE(
      provider.GetBooleanEvaluation("flag1", false, empty_ctx_)->GetValue());
  EXPECT_FALSE(provider.GetBooleanEvaluation("common_flag", true, empty_ctx_)
                   ->GetValue());

  std::unordered_map<std::string, std::any> updated;
  updated["flag2"] = CreateBoolFlag({{"off", false}}, "off");
  updated["common_flag"] = CreateBoolFlag({{"updated", true}}, "updated");

  provider.UpdateFlags(std::move(updated));

  // flag1 should still exist and retain its value as it was not in
  // `updated_flags_map`
  std::unique_ptr<BoolResolutionDetails> flag1_res =
      provider.GetBooleanEvaluation("flag1", false, empty_ctx_);
  EXPECT_TRUE(flag1_res->GetValue());
  EXPECT_EQ(flag1_res->GetReason(), Reason::kStatic);
  EXPECT_THAT(flag1_res->GetVariant(), Optional(std::string("on")));

  // flag2 should now exist and be evaluated to its new value
  std::unique_ptr<BoolResolutionDetails> flag2_res =
      provider.GetBooleanEvaluation("flag2", true, empty_ctx_);
  EXPECT_FALSE(flag2_res->GetValue());
  EXPECT_EQ(flag2_res->GetReason(), Reason::kStatic);
  EXPECT_THAT(flag2_res->GetVariant(), Optional(std::string("off")));

  // common_flag should be updated to true
  std::unique_ptr<BoolResolutionDetails> common_flag_res =
      provider.GetBooleanEvaluation("common_flag", false, empty_ctx_);
  EXPECT_TRUE(common_flag_res->GetValue());
  EXPECT_EQ(common_flag_res->GetReason(), Reason::kStatic);
  EXPECT_THAT(common_flag_res->GetVariant(), Optional(std::string("updated")));
}

TEST_F(InMemoryProviderTest, NoDefaultVariantAndEvaluatorFailsOrMissing) {
  InMemoryProvider provider1({});
  provider1.UpdateFlag("no_default_no_evaluator",
                       CreateBoolFlag({{"v1", true}}, std::nullopt, nullptr));
  EXPECT_TRUE(provider1.Init(empty_ctx_).ok());
  std::unique_ptr<BoolResolutionDetails> res1 = provider1.GetBooleanEvaluation(
      "no_default_no_evaluator", true, empty_ctx_);
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
      CreateBoolFlag({{"v1", true}}, std::nullopt, failing_evaluator));
  EXPECT_TRUE(provider2.Init(empty_ctx_).ok());
  std::unique_ptr<BoolResolutionDetails> res2 = provider2.GetBooleanEvaluation(
      "no_default_failing_evaluator", false, empty_ctx_);
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
                      CreateBoolFlag({{"on", true}, {"off", false}}, "off",
                                     context_aware_evaluator));
  EXPECT_TRUE(provider.Init(empty_ctx_).ok());

  EvaluationContext admin_ctx =
      EvaluationContext::Builder().WithAttribute("user_is_admin", true).build();
  std::unique_ptr<BoolResolutionDetails> res_admin =
      provider.GetBooleanEvaluation("admin_flag", false, admin_ctx);
  ASSERT_NE(res_admin, nullptr);
  EXPECT_TRUE(res_admin->GetValue());
  EXPECT_EQ(res_admin->GetReason(), Reason::kTargetingMatch);
  EXPECT_FALSE(res_admin->GetErrorCode().has_value());

  EvaluationContext non_admin_ctx = EvaluationContext::Builder()
                                        .WithAttribute("user_is_admin", false)
                                        .build();
  std::unique_ptr<BoolResolutionDetails> res_non_admin =
      provider.GetBooleanEvaluation("admin_flag", true, non_admin_ctx);
  ASSERT_NE(res_non_admin, nullptr);
  EXPECT_FALSE(res_non_admin->GetValue());
  EXPECT_EQ(res_non_admin->GetReason(), Reason::kTargetingMatch);
  EXPECT_FALSE(res_non_admin->GetErrorCode().has_value());

  std::unique_ptr<BoolResolutionDetails> res_no_attr =
      provider.GetBooleanEvaluation("admin_flag", true, empty_ctx_);
  ASSERT_NE(res_no_attr, nullptr);
  EXPECT_FALSE(res_no_attr->GetValue());
  EXPECT_EQ(res_no_attr->GetReason(), Reason::kTargetingMatch);
  EXPECT_FALSE(res_no_attr->GetErrorCode().has_value());

  EvaluationContext wrong_type_ctx =
      EvaluationContext::Builder()
          .WithAttribute("user_is_admin", std::string("true"))
          .build();
  std::unique_ptr<BoolResolutionDetails> res_wrong_type =
      provider.GetBooleanEvaluation("admin_flag", true, wrong_type_ctx);
  ASSERT_NE(res_wrong_type, nullptr);
  EXPECT_FALSE(res_wrong_type->GetValue());
  EXPECT_EQ(res_wrong_type->GetReason(), Reason::kDefault);
  EXPECT_THAT(res_wrong_type->GetVariant(), Optional(std::string("off")));
  EXPECT_FALSE(res_wrong_type->GetErrorCode().has_value());
}
}  // namespace openfeature
