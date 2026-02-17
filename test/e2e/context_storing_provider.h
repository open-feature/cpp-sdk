#ifndef CPP_SDK_INCLUDE_TEST_E2E_CONTEXT_STORING_PROVIDER_H_
#define CPP_SDK_INCLUDE_TEST_E2E_CONTEXT_STORING_PROVIDER_H_

#include "openfeature/provider.h"
#include "openfeature/evaluation_context.h"
#include "openfeature/resolution_details.h"
#include "openfeature/metadata.h"

#include <string>
#include <memory>

namespace openfeature_e2e {

// A simple provider that stores the last evaluation context it received.
class ContextStoringProvider : public openfeature::FeatureProvider {
public:
    ~ContextStoringProvider() override = default;

    openfeature::Metadata GetMetadata() const override;

    std::unique_ptr<openfeature::BoolResolutionDetails> GetBooleanEvaluation(
        std::string_view key, bool default_value, const openfeature::EvaluationContext& ctx) override;
   
private:
    openfeature::EvaluationContext last_evaluation_context_ = openfeature::EvaluationContext::Builder().build();
};

} // namespace openfeature_e2e
#endif // CPP_SDK_INCLUDE_TEST_E2E_CONTEXT_STORING_PROVIDER_H_
