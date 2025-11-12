#ifndef CPP_SDK_INCLUDE_OPENFEATURE_PROVIDER_H_
#define CPP_SDK_INCLUDE_OPENFEATURE_PROVIDER_H_

#include "openfeature/provider_evaluation.h"
#include "openfeature/evaluation_context.h"
#include "openfeature/metadata.h"

#include <memory>
#include <string_view>

namespace openfeature {

// FeatureProvider interface defines a set of functions that can be called
// in order to evaluate a flag. This should be implemented by flag management systems.
// https://openfeature.dev/specification/sections/providers#21-feature-provider-interface
class FeatureProvider{
 public:
  virtual ~FeatureProvider() = default;
  virtual Metadata GetMetadata() const = 0;
  virtual std::unique_ptr<ProviderEvaluation<bool>> GetBooleanEvaluation(std::string_view flag, bool default_value, const EvaluationContext& ctx) = 0;

  //TODO: Add other flag types (e.g. string, int, float, object)
};

} // namespace openfeature

#endif CPP_SDK_INCLUDE_OPENFEATURE_PROVIDER_H_
