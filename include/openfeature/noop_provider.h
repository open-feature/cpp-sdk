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

class NoopProvider : public FeatureProvider {
 public:
  NoopProvider();
  Metadata GetMetadata() const override;
  std::unique_ptr<BoolResolutionDetails> GetBooleanEvaluation(
      std::string_view flag, bool default_value,
      const EvaluationContext& ctx) override;

  // TODO: Add other flag types (e.g. string, int, float, object)

 private:
  std::string name_ = "Noop Provider";
};

}  // namespace openfeature

#endif  // CPP_SDK_INCLUDE_OPENFEATURE_NOOP_PROVIDER_H_
