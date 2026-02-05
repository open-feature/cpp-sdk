#ifndef CPP_SDK_INCLUDE_OPENFEATURE_NOOP_PROVIDER_H_
#define CPP_SDK_INCLUDE_OPENFEATURE_NOOP_PROVIDER_H_

#include <memory>
#include <string>
#include <string_view>

#include "openfeature/evaluation_context.h"
#include "openfeature/metadata.h"
#include "openfeature/provider.h"
#include "openfeature/resolution_details.h"
#include "openfeature/value.h"

namespace openfeature {

// This class implements the FeatureProvider interface and returns the supplied
// default flag value.
class NoopProvider : public FeatureProvider {
 public:
  NoopProvider() = default;

  // Metadata returns the metadata of the provider.
  Metadata GetMetadata() const override;

  // BooleanEvaluation returns a boolean flag.
  std::unique_ptr<BoolResolutionDetails> GetBooleanEvaluation(
      std::string_view flag, bool default_value,
      const EvaluationContext& ctx) override;

  // StringResolutionDetails returns a string flag.
  std::unique_ptr<StringResolutionDetails> GetStringEvaluation(
      std::string_view flag, std::string_view default_value,
      const EvaluationContext& ctx);

  // IntResolutionDetails returns an integer flag.
  std::unique_ptr<IntResolutionDetails> GetIntegerEvaluation(
      std::string_view flag, int64_t default_value,
      const EvaluationContext& ctx);

  // DoubleResolutionDetails returns a double flag.
  std::unique_ptr<DoubleResolutionDetails> GetDoubleEvaluation(
      std::string_view flag, double default_value,
      const EvaluationContext& ctx);

  // ObjectResolutionDetails returns an object flag.
  std::unique_ptr<ObjectResolutionDetails> GetObjectEvaluation(
      std::string_view flag, Value default_value, const EvaluationContext& ctx);

 private:
  std::string name_ = "Noop Provider";
};

}  // namespace openfeature

#endif  // CPP_SDK_INCLUDE_OPENFEATURE_NOOP_PROVIDER_H_
