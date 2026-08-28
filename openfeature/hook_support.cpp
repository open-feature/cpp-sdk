#include "openfeature/hook_support.h"

namespace openfeature {

std::vector<std::shared_ptr<GeneralHook>> HookSupport::CollectHooks(
    const std::vector<std::shared_ptr<GeneralHook>>& client_hooks,
    const std::optional<EvaluationOptions>& options,
    const std::shared_ptr<FeatureProvider>& provider) {
  std::vector<std::shared_ptr<GeneralHook>> forward_hooks;
  auto api_hooks = GlobalHookManager::GetInstance().GetHooks();
  auto invocation_hooks = options.has_value()
                              ? options->hooks
                              : std::vector<std::shared_ptr<GeneralHook>>{};
  auto provider_hooks = provider ? provider->GetHooks()
                                 : std::vector<std::shared_ptr<GeneralHook>>{};

  forward_hooks.reserve(api_hooks.size() + client_hooks.size() +
                        invocation_hooks.size() + provider_hooks.size());

  for (auto& hook : api_hooks) {
    if (hook != nullptr) {
      forward_hooks.push_back(std::move(hook));
    }
  }
  for (const auto& hook : client_hooks) {
    if (hook != nullptr) {
      forward_hooks.push_back(hook);
    }
  }
  for (auto& hook : invocation_hooks) {
    if (hook != nullptr) {
      forward_hooks.push_back(std::move(hook));
    }
  }
  for (auto& hook : provider_hooks) {
    if (hook != nullptr) {
      forward_hooks.push_back(std::move(hook));
    }
  }

  return forward_hooks;
}

std::unordered_map<const GeneralHook*, std::shared_ptr<HookData>>
HookSupport::CreateHookDataMap(
    const std::vector<std::shared_ptr<GeneralHook>>& hooks) {
  std::unordered_map<const GeneralHook*, std::shared_ptr<HookData>>
      hook_data_map;
  for (const auto& hook : hooks) {
    if (hook != nullptr &&
        hook_data_map.find(hook.get()) == hook_data_map.end()) {
      hook_data_map[hook.get()] = std::make_shared<HookData>();
    }
  }
  return hook_data_map;
}

}  // namespace openfeature
