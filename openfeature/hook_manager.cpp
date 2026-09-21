#include "openfeature/hook_manager.h"

#include <memory>
#include <mutex>
#include <shared_mutex>
#include <utility>
#include <vector>

#include "openfeature/general_hook.h"

namespace openfeature {

HookManager::HookManager()
    : hooks_(std::make_shared<std::vector<std::shared_ptr<GeneralHook>>>()) {}

HookManager& HookManager::GetInstance() {
  static HookManager instance;
  return instance;
}

void HookManager::AddHooks(std::vector<std::shared_ptr<GeneralHook>> hooks) {
  std::unique_lock<std::shared_mutex> lock(hooks_mutex_);
  auto new_hooks =
      std::make_shared<std::vector<std::shared_ptr<GeneralHook>>>(*hooks_);
  new_hooks->reserve(new_hooks->size() + hooks.size());
  for (auto& hook : hooks) {
    if (hook != nullptr) {
      new_hooks->push_back(std::move(hook));
    }
  }
  hooks_ = std::move(new_hooks);
}

void HookManager::AddHook(std::shared_ptr<GeneralHook> hook) {
  if (hook == nullptr) return;
  std::unique_lock<std::shared_mutex> lock(hooks_mutex_);
  auto new_hooks =
      std::make_shared<std::vector<std::shared_ptr<GeneralHook>>>(*hooks_);
  new_hooks->push_back(std::move(hook));
  hooks_ = std::move(new_hooks);
}

std::shared_ptr<const std::vector<std::shared_ptr<GeneralHook>>>
HookManager::GetHooks() const {
  std::shared_lock<std::shared_mutex> lock(hooks_mutex_);
  return hooks_;
}

void HookManager::ClearHooks() {
  std::unique_lock<std::shared_mutex> lock(hooks_mutex_);
  hooks_ = std::make_shared<std::vector<std::shared_ptr<GeneralHook>>>();
}

}  // namespace openfeature
