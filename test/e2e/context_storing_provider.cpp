#include "test/e2e/context_storing_provider.h"
#include "openfeature/reason.h"
#include "openfeature/flag_metadata.h"

namespace openfeature_e2e {


openfeature::Metadata ContextStoringProvider::GetMetadata() const {
    return openfeature::Metadata{"ContextStoringProvider"};
}

std::unique_ptr<openfeature::BoolResolutionDetails> ContextStoringProvider::GetBooleanEvaluation(
    std::string_view key, bool default_value, const openfeature::EvaluationContext& ctx) {
    // Store a copy of the evaluation context.
    // We need to copy because `ctx` is passed by const reference.
    this->last_evaluation_context_ = ctx;

    return std::make_unique<openfeature::BoolResolutionDetails>(
        default_value,                 // The default value
        openfeature::Reason::kDefault,              // Reason for resolution
        "default-variant",             // A generic variant identifier
        openfeature::FlagMetadata(),                // Empty metadata
        std::nullopt,                  // No error code
        ""                             // Empty error message
    );
}
   

} // namespace openfeature_e2e
