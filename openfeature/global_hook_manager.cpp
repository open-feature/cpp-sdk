#include "openfeature/global_hook_manager.h"

#include <memory>
#include <mutex>
#include <shared_mutex>
#include <utility>
#include <vector>

#include "openfeature/general_hook.h"

namespace openfeature {

GlobalHookManager& GlobalHookManager::GetInstance() {
  static GlobalHookManager instance;
  return instance;
}

void GlobalHookManager::AddHooks(
    std::vector<std::shared_ptr<GeneralHook>> hooks) {
  std::unique_lock<std::shared_mutex> lock(hooks_mutex_);
  hooks_.reserve(hooks_.size() + hooks.size());
  for (auto& hook : hooks) {
    if (hook != nullptr) {
      hooks_.push_back(std::move(hook));
    }
  }
}

void GlobalHookManager::AddHook(std::shared_ptr<GeneralHook> hook) {
  if (hook == nullptr) return;
  std::unique_lock<std::shared_mutex> lock(hooks_mutex_);
  hooks_.push_back(std::move(hook));
}

std::vector<std::shared_ptr<GeneralHook>> GlobalHookManager::GetHooks() const {
  std::shared_lock<std::shared_mutex> lock(hooks_mutex_);
  return hooks_;
}

void GlobalHookManager::ClearHooks() {
  std::unique_lock<std::shared_mutex> lock(hooks_mutex_);
  hooks_.clear();
}

}  // namespace openfeature
