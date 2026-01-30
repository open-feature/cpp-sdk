#include "openfeature/global_context_manager.h"

#include <mutex>

namespace openfeature {

GlobalContextManager& GlobalContextManager::GetInstance() {
  static GlobalContextManager instance;
  return instance;
}

void GlobalContextManager::SetGlobalEvaluationContext(
    const EvaluationContext& ctx) {
  std::unique_lock<std::shared_mutex> lock(context_mutex_);
  global_evaluation_context_ = ctx;
}

EvaluationContext GlobalContextManager::GetGlobalEvaluationContext() const {
  std::shared_lock<std::shared_mutex> lock(context_mutex_);
  return global_evaluation_context_;
}

}  // namespace openfeature