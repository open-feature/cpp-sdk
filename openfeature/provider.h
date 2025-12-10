#ifndef CPP_SDK_INCLUDE_OPENFEATURE_PROVIDER_H_
#define CPP_SDK_INCLUDE_OPENFEATURE_PROVIDER_H_

#include <memory>
#include <string_view>

#include "absl/status/status.h"
#include "openfeature/evaluation_context.h"
#include "openfeature/metadata.h"
#include "openfeature/resolution_details.h"

namespace openfeature {

// FeatureProvider interface defines a set of functions that can be called
// in order to evaluate a flag. This should be implemented by flag management
// systems.
// https://openfeature.dev/specification/sections/providers#21-feature-provider-interface
class FeatureProvider {
 public:
  virtual ~FeatureProvider() = default;
  virtual Metadata GetMetadata() const = 0;
  virtual std::unique_ptr<BoolResolutionDetails> GetBooleanEvaluation(
      std::string_view flag, bool default_value,
      const EvaluationContext& ctx) = 0;
  virtual absl::Status Init(const EvaluationContext& ctx) {
    return absl::OkStatus();
  }
  virtual absl::Status Shutdown() { return absl::OkStatus(); }

  // TODO: Add other flag types (e.g. string, int, float, object)
};

}  // namespace openfeature

#endif  // CPP_SDK_INCLUDE_OPENFEATURE_PROVIDER_H_
