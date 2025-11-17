#include "openfeature/provider.h"

std::unique_ptr<openfeature::ProviderEvuation<bool>> GetBooleanEvaluation(
    const std::string_view flag, bool defaultValue,
    const openfeature::EvaluationContext& ctx) {
  return nullptr;
}