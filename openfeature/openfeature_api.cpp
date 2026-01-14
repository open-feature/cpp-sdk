#include "openfeature/openfeature_api.h"

#include "openfeature/client_api.h"

namespace openfeature {

OpenFeatureAPI::OpenFeatureAPI() {
  // provider_repository_ is automatically constructed.
  // It guarantees a NoopProvider is set by default.
  // TODO: init hooks & events.
}

OpenFeatureAPI::~OpenFeatureAPI() { Shutdown(); }

OpenFeatureAPI& OpenFeatureAPI::GetInstance() {
  static OpenFeatureAPI instance;
  return instance;
}

void OpenFeatureAPI::SetProvider(std::shared_ptr<FeatureProvider> provider) {
  provider_repository_.SetProvider(provider, evaluation_context_, false);
}

void OpenFeatureAPI::SetProvider(std::string_view domain,
                                 std::shared_ptr<FeatureProvider> provider) {
  provider_repository_.SetProvider(domain, provider, evaluation_context_,
                                   false);
}

void OpenFeatureAPI::SetProviderAndWait(
    std::shared_ptr<FeatureProvider> provider) {
  provider_repository_.SetProvider(provider, evaluation_context_, true);
}

void OpenFeatureAPI::SetProviderAndWait(
    std::string_view domain, std::shared_ptr<FeatureProvider> provider) {
  provider_repository_.SetProvider(domain, provider, evaluation_context_, true);
}

std::shared_ptr<FeatureProvider> OpenFeatureAPI::GetProvider(
    std::string_view domain) const {
  return provider_repository_.GetProvider(domain);
}

std::shared_ptr<Client> OpenFeatureAPI::GetClient() { return GetClient(""); }

std::shared_ptr<Client> OpenFeatureAPI::GetClient(std::string_view domain) {
  auto client = std::make_shared<ClientAPI>(this, domain);
  return client;
}

void OpenFeatureAPI::SetEvaluationContext(const EvaluationContext& ctx) {
  std::unique_lock<std::shared_mutex> lock(context_mutex_);
  evaluation_context_ = ctx;
}

EvaluationContext OpenFeatureAPI::GetEvaluationContext() const {
  std::shared_lock<std::shared_mutex> lock(context_mutex_);
  return evaluation_context_;
}

Metadata OpenFeatureAPI::GetProviderMetadata(std::string_view domain) const {
  std::shared_ptr<FeatureProvider> provider =
      provider_repository_.GetProvider(domain);
  if (provider) {
    return provider->GetMetadata();
  }
  return Metadata();  // Return empty metadata if provider not found
}

ProviderStatus OpenFeatureAPI::GetProviderStatus(
    std::string_view domain) const {
  return provider_repository_.GetProviderStatus(domain);
}

void OpenFeatureAPI::Shutdown() { provider_repository_.Shutdown(); }

}  // namespace openfeature