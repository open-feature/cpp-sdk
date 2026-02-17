#ifndef CPP_SDK_INCLUDE_TEST_E2E_STATE_H_
#define CPP_SDK_INCLUDE_TEST_E2E_STATE_H_

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "openfeature/client.h"
#include "openfeature/evaluation_context.h"
#include "openfeature/provider.h"
#include "openfeature/resolution_details.h"  // For ResolutionDetails<Value>
#include "test/e2e/flag.h"

namespace openfeature_e2e {

struct ScenarioState {
  std::shared_ptr<openfeature::Client> client;
  Flag flag;
  openfeature::EvaluationContext ctx =
      openfeature::EvaluationContext::Builder().build();
  // FlagEvaluationDetails eval
  // MockHook hook
  std::shared_ptr<openfeature::FeatureProvider> provider;
  openfeature::EvaluationContext invocation_ctx =
      openfeature::EvaluationContext::Builder().build();
  std::vector<std::string> levels;
};

}  // namespace openfeature_e2e
#endif  // CPP_SDK_INCLUDE_TEST_E2E_STATE_H_