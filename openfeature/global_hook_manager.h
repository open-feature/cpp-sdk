#ifndef CPP_SDK_INCLUDE_OPENFEATURE_GLOBAL_HOOK_MANAGER_H_
#define CPP_SDK_INCLUDE_OPENFEATURE_GLOBAL_HOOK_MANAGER_H_

#include <memory>
#include <shared_mutex>
#include <vector>

#include "openfeature/general_hook.h"

namespace openfeature {

// Manages global hooks for the OpenFeature SDK.
// Provides thread-safe hook storage and access across the application.
class GlobalHookManager {
 public:
  static GlobalHookManager& GetInstance();

  GlobalHookManager(const GlobalHookManager&) = delete;
  GlobalHookManager& operator=(const GlobalHookManager&) = delete;

  // Adds one or more global hooks, appending them to existing hooks.
  // Filters out nullptr entries.
  void AddHooks(std::vector<std::shared_ptr<GeneralHook>> hooks);

  // Adds a single global hook. Filters out nullptr entries.
  void AddHook(std::shared_ptr<GeneralHook> hook);

  // Retrieves all configured global hooks.
  std::vector<std::shared_ptr<GeneralHook>> GetHooks() const;

  // Clears all configured global hooks.
  void ClearHooks();

 private:
  GlobalHookManager() = default;
  mutable std::shared_mutex hooks_mutex_;
  std::vector<std::shared_ptr<GeneralHook>> hooks_;
};

}  // namespace openfeature

#endif  // CPP_SDK_INCLUDE_OPENFEATURE_GLOBAL_HOOK_MANAGER_H_
