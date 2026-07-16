#include "openfeature/openfeature_api.h"

#include "openfeature/client_api.h"
#include "openfeature/global_context_manager.h"

namespace openfeature {

OpenFeatureAPI::OpenFeatureAPI() = default;

OpenFeatureAPI& OpenFeatureAPI::GetInstance() {
  static OpenFeatureAPI instance;
  return instance;
}

void OpenFeatureAPI::SetProvider(std::shared_ptr<FeatureProvider> provider) {
  provider_repository_.SetProvider(
      provider,
      GlobalContextManager::GetInstance().GetGlobalEvaluationContext(), false);
}

void OpenFeatureAPI::SetProvider(std::string_view domain,
                                 std::shared_ptr<FeatureProvider> provider) {
  provider_repository_.SetProvider(
      domain, provider,
      GlobalContextManager::GetInstance().GetGlobalEvaluationContext(), false);
}

void OpenFeatureAPI::SetProviderAndWait(
    std::shared_ptr<FeatureProvider> provider) {
  provider_repository_.SetProvider(
      provider,
      GlobalContextManager::GetInstance().GetGlobalEvaluationContext(), true);
}

void OpenFeatureAPI::SetProviderAndWait(
    std::string_view domain, std::shared_ptr<FeatureProvider> provider) {
  provider_repository_.SetProvider(
      domain, provider,
      GlobalContextManager::GetInstance().GetGlobalEvaluationContext(), true);
}

std::shared_ptr<FeatureProvider> OpenFeatureAPI::GetProvider() const {
  return GetProvider("");
}

std::shared_ptr<FeatureProvider> OpenFeatureAPI::GetProvider(
    std::string_view domain) const {
  return provider_repository_.GetProvider(domain);
}

std::shared_ptr<Client> OpenFeatureAPI::GetClient() { return GetClient(""); }

std::shared_ptr<Client> OpenFeatureAPI::GetClient(std::string_view domain) {
  auto client = std::make_shared<ClientAPI>(provider_repository_, domain);
  return client;
}

void OpenFeatureAPI::SetEvaluationContext(const EvaluationContext& ctx) {
  GlobalContextManager::GetInstance().SetGlobalEvaluationContext(ctx);
}

EvaluationContext OpenFeatureAPI::GetEvaluationContext() const {
  return GlobalContextManager::GetInstance().GetGlobalEvaluationContext();
}

Metadata OpenFeatureAPI::GetProviderMetadata() const {
  return GetProviderMetadata("");
}

Metadata OpenFeatureAPI::GetProviderMetadata(std::string_view domain) const {
  std::shared_ptr<FeatureProvider> provider =
      provider_repository_.GetProvider(domain);
  if (provider) {
    return provider->GetMetadata();
  }
  return {};  // Return empty metadata if provider not found
}

ProviderStatus OpenFeatureAPI::GetProviderStatus() const {
  return GetProviderStatus("");
}

ProviderStatus OpenFeatureAPI::GetProviderStatus(
    std::string_view domain) const {
  return provider_repository_.GetProviderStatus(domain);
}

void OpenFeatureAPI::AddHooks(std::vector<std::shared_ptr<BaseHook>> hooks) {
  std::unique_lock lock(hooks_mutex_);
  hooks_.reserve(hooks_.size() + hooks.size());
  for (auto& hook : hooks) {
    if (hook != nullptr) {
      hooks_.push_back(std::move(hook));
    }
  }
}

void OpenFeatureAPI::AddHook(std::shared_ptr<BaseHook> hook) {
  if (hook == nullptr) return;
  std::unique_lock lock(hooks_mutex_);
  hooks_.push_back(std::move(hook));
}

std::vector<std::shared_ptr<BaseHook>> OpenFeatureAPI::GetHooks() const {
  std::shared_lock lock(hooks_mutex_);
  return hooks_;
}

void OpenFeatureAPI::Shutdown() {
  provider_repository_.Shutdown();
  std::unique_lock lock(hooks_mutex_);
  hooks_.clear();
}

}  // namespace openfeature