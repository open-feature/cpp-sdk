#include "client_api.h"

#include <utility>

#include "openfeature/flag_metadata.h"
#include "openfeature/global_context_manager.h"
#include "openfeature/reason.h"

namespace openfeature {

ClientAPI::ClientAPI(std::string_view domain) : domain_(domain) {}

Metadata ClientAPI::GetMetadata() { return Metadata{domain_}; }

EvaluationContext ClientAPI::GetEvaluationContext() {
  std::lock_guard<std::mutex> lock(context_mutex_);
  return evaluation_context_;
}

void ClientAPI::SetEvaluationContext(const EvaluationContext& ctx) {
  std::lock_guard<std::mutex> lock(context_mutex_);
  evaluation_context_ = ctx;
}

ProviderStatus ClientAPI::GetProviderStatus() {
  return provider_repository_.GetProviderStatus(domain_);
}

bool ClientAPI::GetBooleanValue(std::string_view flag_key, bool default_value) {
  return EvaluateBooleanFlag(flag_key, default_value, std::nullopt)->GetValue();
}

bool ClientAPI::GetBooleanValue(std::string_view flag_key, bool default_value,
                                const EvaluationContext& ctx) {
  return EvaluateBooleanFlag(flag_key, default_value, ctx)->GetValue();
}

std::unique_ptr<BoolResolutionDetails> ClientAPI::EvaluateBooleanFlag(
    std::string_view flag_key, bool default_value,
    const std::optional<EvaluationContext>& ctx) {
  if (GetProviderStatus() != ProviderStatus::kReady) {
    return std::make_unique<BoolResolutionDetails>(
        default_value, Reason::kError, std::nullopt, FlagMetadata(),
        ErrorCode::kProviderNotReady, "Provider is not ready");
  }

  std::shared_ptr<FeatureProvider> provider =
      provider_repository_.GetProvider(domain_);
  if (!provider) {
    return std::make_unique<BoolResolutionDetails>(
        default_value, Reason::kError, std::nullopt, FlagMetadata(),
        ErrorCode::kProviderFatal, "Provider not found for domain");
  }

  EvaluationContext merged_context = MergeContexts(ctx);
  return provider->GetBooleanEvaluation(flag_key, default_value,
                                        merged_context);
}

EvaluationContext ClientAPI::MergeContexts(
    const std::optional<EvaluationContext>& invocation_ctx) {
  // EvaluationContext api_context = api_->GetEvaluationContext();
  EvaluationContext api_context =
      GlobalContextManager::GetInstance().GetGlobalEvaluationContext();
  EvaluationContext client_context = this->GetEvaluationContext();
  // TODO: Add context merging logic after EvaluationContext is implemented

  if (invocation_ctx) {
    return *invocation_ctx;
  }
  return GetEvaluationContext();
}

}  // namespace openfeature