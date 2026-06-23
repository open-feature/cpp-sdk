#ifndef CPP_SDK_INCLUDE_OPENFEATURE_PROVIDER_H_
#define CPP_SDK_INCLUDE_OPENFEATURE_PROVIDER_H_

#include <memory>
#include <string_view>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "openfeature/evaluation_context.h"
#include "openfeature/metadata.h"
#include "openfeature/resolution_details.h"
#include "openfeature/value.h"

namespace openfeature {

// FeatureProvider interface defines a set of functions that can be called
// in order to evaluate a flag. This should be implemented by flag management
// systems.
// Implementations must NOT throw C++ exceptions. All errors
// must be reported by returning an absl::Status inside the absl::StatusOr.
// Throwing an exception will cause abnormal termination of the application.
// https://openfeature.dev/specification/sections/providers#21-feature-provider-interface
class FeatureProvider {
 public:
  virtual ~FeatureProvider() = default;
  virtual Metadata GetMetadata() const = 0;
  virtual absl::StatusOr<std::unique_ptr<BoolResolutionDetails>>
  GetBooleanEvaluation(std::string_view flag, bool default_value,
                       const EvaluationContext& ctx) = 0;
  virtual absl::StatusOr<std::unique_ptr<StringResolutionDetails>>
  GetStringEvaluation(std::string_view flag, std::string_view default_value,
                      const EvaluationContext& ctx) = 0;
  virtual absl::StatusOr<std::unique_ptr<IntResolutionDetails>>
  GetIntegerEvaluation(std::string_view flag, int64_t default_value,
                       const EvaluationContext& ctx) = 0;
  virtual absl::StatusOr<std::unique_ptr<DoubleResolutionDetails>>
  GetDoubleEvaluation(std::string_view flag, double default_value,
                      const EvaluationContext& ctx) = 0;
  virtual absl::StatusOr<std::unique_ptr<ObjectResolutionDetails>>
  GetObjectEvaluation(std::string_view flag, Value default_value,
                      const EvaluationContext& ctx) = 0;
  virtual absl::Status Init(const EvaluationContext& ctx) {
    return absl::OkStatus();
  }
  virtual absl::Status Shutdown() { return absl::OkStatus(); }
};

}  // namespace openfeature

#endif  // CPP_SDK_INCLUDE_OPENFEATURE_PROVIDER_H_
