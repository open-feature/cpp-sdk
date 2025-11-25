#ifndef CPP_SDK_INCLUDE_OPENFEATURE_NOOP_PROVIDER_H_
#define CPP_SDK_INCLUDE_OPENFEATURE_NOOP_PROVIDER_H_

#include <memory>
#include <string>
#include <string_view>

#include "openfeature/evaluation_context.h"
#include "openfeature/metadata.h"
#include "openfeature/provider.h"
#include "openfeature/resolution_details.h"

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

  // TODO: Add other flag types (e.g. string, int, float, object)

 private:
  std::string name_ = "Noop Provider";
};

}  // namespace openfeature

#endif  // CPP_SDK_INCLUDE_OPENFEATURE_NOOP_PROVIDER_H_
