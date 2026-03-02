#include "openfeature/noop_provider.h"

namespace openfeature {

Metadata NoopProvider::GetMetadata() const { return Metadata{name_}; }

std::unique_ptr<BoolResolutionDetails> NoopProvider::GetBooleanEvaluation(
    std::string_view flag, bool default_value, const EvaluationContext& ctx) {
  return std::make_unique<BoolResolutionDetails>(
      default_value, Reason::kDefault, "default-variant", FlagMetadata(),
      std::nullopt, "");
}

std::unique_ptr<StringResolutionDetails> NoopProvider::GetStringEvaluation(
    std::string_view flag, std::string_view default_value,
    const EvaluationContext& ctx) {
  return std::make_unique<StringResolutionDetails>(
      std::string(default_value), Reason::kDefault, "default-variant",
      FlagMetadata(), std::nullopt, "");
}

std::unique_ptr<IntResolutionDetails> NoopProvider::GetIntegerEvaluation(
    std::string_view flag, int64_t default_value,
    const EvaluationContext& ctx) {
  return std::make_unique<IntResolutionDetails>(
      default_value, Reason::kDefault, "default-variant", FlagMetadata(),
      std::nullopt, "");
}

std::unique_ptr<DoubleResolutionDetails> NoopProvider::GetDoubleEvaluation(
    std::string_view flag, double default_value, const EvaluationContext& ctx) {
  return std::make_unique<DoubleResolutionDetails>(
      default_value, Reason::kDefault, "default-variant", FlagMetadata(),
      std::nullopt, "");
}

std::unique_ptr<ObjectResolutionDetails> NoopProvider::GetObjectEvaluation(
    std::string_view flag, Value default_value, const EvaluationContext& ctx) {
  return std::make_unique<ObjectResolutionDetails>(
      default_value, Reason::kDefault, "default-variant", FlagMetadata(),
      std::nullopt, "");
}

}  // namespace openfeature
