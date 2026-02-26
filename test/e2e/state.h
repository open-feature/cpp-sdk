#ifndef CPP_SDK_INCLUDE_TEST_E2E_STATE_H_
#define CPP_SDK_INCLUDE_TEST_E2E_STATE_H_

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "openfeature/client.h"
#include "openfeature/evaluation_context.h"
#include "openfeature/provider.h"
#include "openfeature/resolution_details.h"
#include "openfeature/value.h"
#include "test/e2e/flag_test.h"

namespace openfeature_e2e {

struct State {
  std::shared_ptr<openfeature::Client> client;
  std::shared_ptr<openfeature::FeatureProvider> provider;
  FlagTest flag;
  std::unique_ptr<openfeature::EvaluationContext> context;
  std::unique_ptr<openfeature::EvaluationContext> invocation_context;
  std::vector<std::string> levels;

  openfeature::Value last_evaluation_value;
};

// TODO: Update struct after implementing hooks and flag evaluation details.
}  // namespace openfeature_e2e
#endif  // CPP_SDK_INCLUDE_TEST_E2E_STATE_H_