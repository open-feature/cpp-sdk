#ifndef CPP_SDK_INCLUDE_OPENFEATURE_EVALUATION_OPTIONS_H_
#define CPP_SDK_INCLUDE_OPENFEATURE_EVALUATION_OPTIONS_H_
#include <memory>
#include <vector>

#include "openfeature/base_hook.h"
#include "openfeature/hook_hints.h"
namespace openfeature {
struct EvaluationOptions {
  std::vector<std::shared_ptr<BaseHook>> hooks;
  HookHints hook_hints;
};
}  // namespace openfeature
#endif  // CPP_SDK_INCLUDE_OPENFEATURE_EVALUATION_OPTIONS_H_