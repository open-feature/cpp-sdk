#ifndef CPP_SDK_INCLUDE_OPENFEATURE_PROVIDER_H_
#define CPP_SDK_INCLUDE_OPENFEATURE_PROVIDER_H_ 

#include "openfeature/provider_evaluation.h"
#include "openfeature/evaluation_context.h"
#include "openfeature/metadata.h"

#include <memory>
#include <string>

namespace openfeature {

// FeatureProvider interface defines a set of functions that can be called
// in order to evaluate a flag. This should be implemented by flag management systems.
// https://openfeature.dev/specification/sections/providers#21-feature-provider-interface
class FeatureProvider{
 public:
  virtual ~FeatureProvider() = default;
  virtual Metadata GetMetadata() const = 0;
  virtual std::unique_ptr<ProviderEvaluation<bool>> GetBooleanEvaluation(const std::string& flag, bool defaultValue, const EvaluationContext& ctx) = 0;
  virtual std::unique_ptr<ProviderEvaluation<std::string>> GetStringEvaluation(const std::string& flag, const std::string& defaultValue, const EvaluationContext& ctx) = 0;
  virtual std::unique_ptr<ProviderEvaluation<double>> GetDoubleEvaluation(const std::string& flag, double defaultValue, const EvaluationContext& ctx) = 0;
  virtual std::unique_ptr<ProviderEvaluation<int64_t>> GetIntEvaluation(const std::string& flag, int64_t defaultValue, const EvaluationContext& ctx) = 0;
  virtual void Initialize(const EvaluationContext& ctx) = 0;
  virtual void Shutdown() = 0;
};

} // namespace openfeature

#endif  CPP_SDK_INCLUDE_OPENFEATURE_PROVIDER_H_