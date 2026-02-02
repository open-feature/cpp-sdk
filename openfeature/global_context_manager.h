#ifndef CPP_SDK_INCLUDE_OPENFEATURE_GLOBAL_CONTEXT_MANAGER_H_
#define CPP_SDK_INCLUDE_OPENFEATURE_GLOBAL_CONTEXT_MANAGER_H_

#include <memory>
#include <shared_mutex>

#include "openfeature/evaluation_context.h"

namespace openfeature {

// Manages the global Evaluation Context for the OpenFeature SDK.
// This data is static across the application (unless explicitly changed)
class GlobalContextManager {
 public:
  static GlobalContextManager& GetInstance();

  GlobalContextManager(const GlobalContextManager&) = delete;
  GlobalContextManager& operator=(const GlobalContextManager&) = delete;

  // Updates the global evaluation context.
  void SetGlobalEvaluationContext(const EvaluationContext& ctx);

  // Retrieves the current global evaluation context.
  EvaluationContext GetGlobalEvaluationContext() const;

 private:
  GlobalContextManager();
  EvaluationContext global_evaluation_context_;
  mutable std::shared_mutex context_mutex_;
};

}  // namespace openfeature

#endif  // CPP_SDK_INCLUDE_OPENFEATURE_GLOBAL_CONTEXT_MANAGER_H_