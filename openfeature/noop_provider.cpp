#include "openfeature/noop_provider.h"

namespace openfeature {

Metadata NoopProvider::GetMetadata() const { return Metadata{name_}; }

std::unique_ptr<BoolResolutionDetails> NoopProvider::GetBooleanEvaluation(
    std::string_view flag, bool default_value, const EvaluationContext& ctx) {
  return std::make_unique<BoolResolutionDetails>(
      default_value, Reason::kDefault, "default-variant", FlagMetadata(),
      std::nullopt, "");
}

}  // namespace openfeature
