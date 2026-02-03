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
                            std::string default_variant,
                            Flag<bool>::ContextEvaluator evaluator = nullptr,
                            bool disabled = false) {
    return Flag<bool>(std::move(variants), std::move(default_variant),
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
}

TEST_F(InMemoryProviderTest, UpdateFlagsReplacesEntireConfiguration) {
  std::unordered_map<std::string, std::any> initial;
  initial["flag1"] = CreateBoolFlag({{"on", true}}, "on");

  InMemoryProvider provider(std::move(initial));
  EXPECT_TRUE(provider.Init(empty_ctx_).ok());

  std::unordered_map<std::string, std::any> updated;
  updated["flag2"] = CreateBoolFlag({{"off", false}}, "off");

  provider.UpdateFlags(std::move(updated));

  EXPECT_EQ(
      provider.GetBooleanEvaluation("flag1", false, empty_ctx_)->GetErrorCode(),
      ErrorCode::kFlagNotFound);
  EXPECT_EQ(
      provider.GetBooleanEvaluation("flag2", true, empty_ctx_)->GetReason(),
      Reason::kStatic);
}
}  // namespace openfeature
