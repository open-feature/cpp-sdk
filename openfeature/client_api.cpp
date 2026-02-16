#include "client_api.h"

#include <utility>

#include "openfeature/flag_metadata.h"
#include "openfeature/global_context_manager.h"
#include "openfeature/reason.h"

namespace openfeature {

ClientAPI::ClientAPI(ProviderRepository& repository, std::string_view domain)
    : provider_repository_(repository),
      domain_(domain),
      evaluation_context_(EvaluationContext::Builder().build()) {}

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

std::string ClientAPI::GetStringValue(std::string_view flag_key,
                                      std::string_view default_value) {
  return EvaluateStringFlag(flag_key, default_value, std::nullopt)->GetValue();
}

std::string ClientAPI::GetStringValue(std::string_view flag_key,
                                      std::string_view default_value,
                                      const EvaluationContext& ctx) {
  return EvaluateStringFlag(flag_key, default_value, ctx)->GetValue();
}

int64_t ClientAPI::GetIntegerValue(std::string_view flag_key,
                                   int64_t default_value) {
  return EvaluateIntegerFlag(flag_key, default_value, std::nullopt)->GetValue();
}

int64_t ClientAPI::GetIntegerValue(std::string_view flag_key,
                                   int64_t default_value,
                                   const EvaluationContext& ctx) {
  return EvaluateIntegerFlag(flag_key, default_value, ctx)->GetValue();
}

double ClientAPI::GetDoubleValue(std::string_view flag_key,
                                 double default_value) {
  return EvaluateDoubleFlag(flag_key, default_value, std::nullopt)->GetValue();
}

double ClientAPI::GetDoubleValue(std::string_view flag_key,
                                 double default_value,
                                 const EvaluationContext& ctx) {
  return EvaluateDoubleFlag(flag_key, default_value, ctx)->GetValue();
}

Value ClientAPI::GetObjectValue(std::string_view flag_key,
                                Value default_value) {
  return EvaluateObjectFlag(flag_key, default_value, std::nullopt)->GetValue();
}

Value ClientAPI::GetObjectValue(std::string_view flag_key, Value default_value,
                                const EvaluationContext& ctx) {
  return EvaluateObjectFlag(flag_key, default_value, ctx)->GetValue();
}

std::unique_ptr<BoolResolutionDetails> ClientAPI::EvaluateBooleanFlag(
    std::string_view flag_key, bool default_value,
    const std::optional<EvaluationContext>& ctx) {
  return this->EvaluateFlag<BoolResolutionDetails>(
      default_value, ctx,
      [&](const std::shared_ptr<FeatureProvider>& provider,
          const EvaluationContext& merged_ctx) {
        return provider->GetBooleanEvaluation(flag_key, default_value,
                                              merged_ctx);
      });
}

std::unique_ptr<StringResolutionDetails> ClientAPI::EvaluateStringFlag(
    std::string_view flag_key, std::string_view default_value,
    const std::optional<EvaluationContext>& ctx) {
  std::string default_str(default_value);
  return this->EvaluateFlag<StringResolutionDetails>(
      default_str, ctx,
      [&](const std::shared_ptr<FeatureProvider>& provider,
          const EvaluationContext& merged_ctx) {
        return provider->GetStringEvaluation(flag_key, default_value,
                                             merged_ctx);
      });
}

std::unique_ptr<IntResolutionDetails> ClientAPI::EvaluateIntegerFlag(
    std::string_view flag_key, int64_t default_value,
    const std::optional<EvaluationContext>& ctx) {
  return this->EvaluateFlag<IntResolutionDetails>(
      default_value, ctx,
      [&](const std::shared_ptr<FeatureProvider>& provider,
          const EvaluationContext& merged_ctx) {
        return provider->GetIntegerEvaluation(flag_key, default_value,
                                              merged_ctx);
      });
}

std::unique_ptr<DoubleResolutionDetails> ClientAPI::EvaluateDoubleFlag(
    std::string_view flag_key, double default_value,
    const std::optional<EvaluationContext>& ctx) {
  return this->EvaluateFlag<DoubleResolutionDetails>(
      default_value, ctx,
      [&](const std::shared_ptr<FeatureProvider>& provider,
          const EvaluationContext& merged_ctx) {
        return provider->GetDoubleEvaluation(flag_key, default_value,
                                             merged_ctx);
      });
}

std::unique_ptr<ObjectResolutionDetails> ClientAPI::EvaluateObjectFlag(
    std::string_view flag_key, Value default_value,
    const std::optional<EvaluationContext>& ctx) {
  return this->EvaluateFlag<ObjectResolutionDetails>(
      default_value, ctx,
      [&](const std::shared_ptr<FeatureProvider>& provider,
          const EvaluationContext& merged_ctx) {
        return provider->GetObjectEvaluation(flag_key, default_value,
                                             merged_ctx);
      });
}

EvaluationContext ClientAPI::MergeContexts(
    const std::optional<EvaluationContext>& invocation_ctx) {
  EvaluationContext global_ctx =
      GlobalContextManager::GetInstance().GetGlobalEvaluationContext();
  EvaluationContext client_ctx = GetEvaluationContext();

  if (invocation_ctx.has_value()) {
    return EvaluationContext::Merge(
        {&global_ctx, &client_ctx, &(*invocation_ctx)});
  } else {
    return EvaluationContext::Merge({&global_ctx, &client_ctx});
  }
}

}  // namespace openfeature
