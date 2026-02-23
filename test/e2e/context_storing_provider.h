#ifndef CPP_SDK_INCLUDE_TEST_E2E_CONTEXT_STORING_PROVIDER_H_
#define CPP_SDK_INCLUDE_TEST_E2E_CONTEXT_STORING_PROVIDER_H_

#include <memory>
#include <string>
#include <string_view>

#include "openfeature/evaluation_context.h"
#include "openfeature/metadata.h"
#include "openfeature/provider.h"
#include "openfeature/resolution_details.h"

namespace openfeature_e2e {

// A simple provider that stores the last evaluation context it received.
class ContextStoringProvider : public openfeature::FeatureProvider {
 public:
  mutable openfeature::EvaluationContext last_ctx =
      openfeature::EvaluationContext::Builder().build();

  ~ContextStoringProvider() override = default;

  openfeature::Metadata GetMetadata() const override;

  std::unique_ptr<openfeature::BoolResolutionDetails> GetBooleanEvaluation(
      std::string_view key, bool default_value,
      const openfeature::EvaluationContext& ctx) override;

  std::unique_ptr<openfeature::StringResolutionDetails> GetStringEvaluation(
      std::string_view key, std::string_view default_value,
      const openfeature::EvaluationContext& ctx) override;

  std::unique_ptr<openfeature::IntResolutionDetails> GetIntegerEvaluation(
      std::string_view key, int64_t default_value,
      const openfeature::EvaluationContext& ctx) override;

  std::unique_ptr<openfeature::DoubleResolutionDetails> GetDoubleEvaluation(
      std::string_view key, double default_value,
      const openfeature::EvaluationContext& ctx) override;

  std::unique_ptr<openfeature::ObjectResolutionDetails> GetObjectEvaluation(
      std::string_view key, openfeature::Value default_value,
      const openfeature::EvaluationContext& ctx) override;
};

}  // namespace openfeature_e2e
#endif  // CPP_SDK_INCLUDE_TEST_E2E_CONTEXT_STORING_PROVIDER_H_
