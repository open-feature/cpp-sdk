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
  std::scoped_lock<std::mutex> lock(context_mutex_);
  return evaluation_context_;
}

void ClientAPI::SetEvaluationContext(const EvaluationContext& ctx) {
  std::scoped_lock<std::mutex> lock(context_mutex_);
  evaluation_context_ = ctx;
}

ProviderStatus ClientAPI::GetProviderStatus() {
  return provider_repository_.GetProviderStatus(domain_);
}

bool ClientAPI::GetBooleanValue(std::string_view flag_key, bool default_value) {
  return EvaluateBooleanFlag(flag_key, default_value, std::nullopt,
                             std::nullopt)
      ->GetValue();
}

bool ClientAPI::GetBooleanValue(std::string_view flag_key, bool default_value,
                                const EvaluationContext& ctx) {
  return EvaluateBooleanFlag(flag_key, default_value, ctx, std::nullopt)
      ->GetValue();
}

bool ClientAPI::GetBooleanValue(std::string_view flag_key, bool default_value,
                                const EvaluationOptions& options) {
  return EvaluateBooleanFlag(flag_key, default_value, std::nullopt, options)
      ->GetValue();
}

bool ClientAPI::GetBooleanValue(std::string_view flag_key, bool default_value,
                                const EvaluationContext& ctx,
                                const EvaluationOptions& options) {
  return EvaluateBooleanFlag(flag_key, default_value, ctx, options)->GetValue();
}

BoolFlagEvaluationDetails ClientAPI::GetBooleanDetails(
    std::string_view flag_key, bool default_value) {
  auto res =
      EvaluateBooleanFlag(flag_key, default_value, std::nullopt, std::nullopt);
  return BoolFlagEvaluationDetails(std::string(flag_key), *res);
}

BoolFlagEvaluationDetails ClientAPI::GetBooleanDetails(
    std::string_view flag_key, bool default_value,
    const EvaluationContext& ctx) {
  auto res = EvaluateBooleanFlag(flag_key, default_value, ctx, std::nullopt);
  return BoolFlagEvaluationDetails(std::string(flag_key), *res);
}

BoolFlagEvaluationDetails ClientAPI::GetBooleanDetails(
    std::string_view flag_key, bool default_value,
    const EvaluationOptions& options) {
  auto res =
      EvaluateBooleanFlag(flag_key, default_value, std::nullopt, options);
  return BoolFlagEvaluationDetails(std::string(flag_key), *res);
}

BoolFlagEvaluationDetails ClientAPI::GetBooleanDetails(
    std::string_view flag_key, bool default_value, const EvaluationContext& ctx,
    const EvaluationOptions& options) {
  auto res = EvaluateBooleanFlag(flag_key, default_value, ctx, options);
  return BoolFlagEvaluationDetails(std::string(flag_key), *res);
}

std::string ClientAPI::GetStringValue(std::string_view flag_key,
                                      std::string_view default_value) {
  return EvaluateStringFlag(flag_key, default_value, std::nullopt, std::nullopt)
      ->GetValue();
}

std::string ClientAPI::GetStringValue(std::string_view flag_key,
                                      std::string_view default_value,
                                      const EvaluationContext& ctx) {
  return EvaluateStringFlag(flag_key, default_value, ctx, std::nullopt)
      ->GetValue();
}

std::string ClientAPI::GetStringValue(std::string_view flag_key,
                                      std::string_view default_value,
                                      const EvaluationOptions& options) {
  return EvaluateStringFlag(flag_key, default_value, std::nullopt, options)
      ->GetValue();
}

std::string ClientAPI::GetStringValue(std::string_view flag_key,
                                      std::string_view default_value,
                                      const EvaluationContext& ctx,
                                      const EvaluationOptions& options) {
  return EvaluateStringFlag(flag_key, default_value, ctx, options)->GetValue();
}

StringFlagEvaluationDetails ClientAPI::GetStringDetails(
    std::string_view flag_key, std::string_view default_value) {
  auto res =
      EvaluateStringFlag(flag_key, default_value, std::nullopt, std::nullopt);
  return StringFlagEvaluationDetails(std::string(flag_key), *res);
}

StringFlagEvaluationDetails ClientAPI::GetStringDetails(
    std::string_view flag_key, std::string_view default_value,
    const EvaluationContext& ctx) {
  auto res = EvaluateStringFlag(flag_key, default_value, ctx, std::nullopt);
  return StringFlagEvaluationDetails(std::string(flag_key), *res);
}

StringFlagEvaluationDetails ClientAPI::GetStringDetails(
    std::string_view flag_key, std::string_view default_value,
    const EvaluationOptions& options) {
  auto res = EvaluateStringFlag(flag_key, default_value, std::nullopt, options);
  return StringFlagEvaluationDetails(std::string(flag_key), *res);
}

StringFlagEvaluationDetails ClientAPI::GetStringDetails(
    std::string_view flag_key, std::string_view default_value,
    const EvaluationContext& ctx, const EvaluationOptions& options) {
  auto res = EvaluateStringFlag(flag_key, default_value, ctx, options);
  return StringFlagEvaluationDetails(std::string(flag_key), *res);
}

int64_t ClientAPI::GetIntegerValue(std::string_view flag_key,
                                   int64_t default_value) {
  return EvaluateIntegerFlag(flag_key, default_value, std::nullopt,
                             std::nullopt)
      ->GetValue();
}

int64_t ClientAPI::GetIntegerValue(std::string_view flag_key,
                                   int64_t default_value,
                                   const EvaluationContext& ctx) {
  return EvaluateIntegerFlag(flag_key, default_value, ctx, std::nullopt)
      ->GetValue();
}

int64_t ClientAPI::GetIntegerValue(std::string_view flag_key,
                                   int64_t default_value,
                                   const EvaluationOptions& options) {
  return EvaluateIntegerFlag(flag_key, default_value, std::nullopt, options)
      ->GetValue();
}

int64_t ClientAPI::GetIntegerValue(std::string_view flag_key,
                                   int64_t default_value,
                                   const EvaluationContext& ctx,
                                   const EvaluationOptions& options) {
  return EvaluateIntegerFlag(flag_key, default_value, ctx, options)->GetValue();
}

IntFlagEvaluationDetails ClientAPI::GetIntegerDetails(std::string_view flag_key,
                                                      int64_t default_value) {
  auto res =
      EvaluateIntegerFlag(flag_key, default_value, std::nullopt, std::nullopt);
  return IntFlagEvaluationDetails(std::string(flag_key), *res);
}

IntFlagEvaluationDetails ClientAPI::GetIntegerDetails(
    std::string_view flag_key, int64_t default_value,
    const EvaluationContext& ctx) {
  auto res = EvaluateIntegerFlag(flag_key, default_value, ctx, std::nullopt);
  return IntFlagEvaluationDetails(std::string(flag_key), *res);
}

IntFlagEvaluationDetails ClientAPI::GetIntegerDetails(
    std::string_view flag_key, int64_t default_value,
    const EvaluationOptions& options) {
  auto res =
      EvaluateIntegerFlag(flag_key, default_value, std::nullopt, options);
  return IntFlagEvaluationDetails(std::string(flag_key), *res);
}

IntFlagEvaluationDetails ClientAPI::GetIntegerDetails(
    std::string_view flag_key, int64_t default_value,
    const EvaluationContext& ctx, const EvaluationOptions& options) {
  auto res = EvaluateIntegerFlag(flag_key, default_value, ctx, options);
  return IntFlagEvaluationDetails(std::string(flag_key), *res);
}

double ClientAPI::GetDoubleValue(std::string_view flag_key,
                                 double default_value) {
  return EvaluateDoubleFlag(flag_key, default_value, std::nullopt, std::nullopt)
      ->GetValue();
}

double ClientAPI::GetDoubleValue(std::string_view flag_key,
                                 double default_value,
                                 const EvaluationContext& ctx) {
  return EvaluateDoubleFlag(flag_key, default_value, ctx, std::nullopt)
      ->GetValue();
}

double ClientAPI::GetDoubleValue(std::string_view flag_key,
                                 double default_value,
                                 const EvaluationOptions& options) {
  return EvaluateDoubleFlag(flag_key, default_value, std::nullopt, options)
      ->GetValue();
}

double ClientAPI::GetDoubleValue(std::string_view flag_key,
                                 double default_value,
                                 const EvaluationContext& ctx,
                                 const EvaluationOptions& options) {
  return EvaluateDoubleFlag(flag_key, default_value, ctx, options)->GetValue();
}

DoubleFlagEvaluationDetails ClientAPI::GetDoubleDetails(
    std::string_view flag_key, double default_value) {
  auto res =
      EvaluateDoubleFlag(flag_key, default_value, std::nullopt, std::nullopt);
  return DoubleFlagEvaluationDetails(std::string(flag_key), *res);
}

DoubleFlagEvaluationDetails ClientAPI::GetDoubleDetails(
    std::string_view flag_key, double default_value,
    const EvaluationContext& ctx) {
  auto res = EvaluateDoubleFlag(flag_key, default_value, ctx, std::nullopt);
  return DoubleFlagEvaluationDetails(std::string(flag_key), *res);
}

DoubleFlagEvaluationDetails ClientAPI::GetDoubleDetails(
    std::string_view flag_key, double default_value,
    const EvaluationOptions& options) {
  auto res = EvaluateDoubleFlag(flag_key, default_value, std::nullopt, options);
  return DoubleFlagEvaluationDetails(std::string(flag_key), *res);
}

DoubleFlagEvaluationDetails ClientAPI::GetDoubleDetails(
    std::string_view flag_key, double default_value,
    const EvaluationContext& ctx, const EvaluationOptions& options) {
  auto res = EvaluateDoubleFlag(flag_key, default_value, ctx, options);
  return DoubleFlagEvaluationDetails(std::string(flag_key), *res);
}

Value ClientAPI::GetObjectValue(std::string_view flag_key,
                                Value default_value) {
  return EvaluateObjectFlag(flag_key, default_value, std::nullopt, std::nullopt)
      ->GetValue();
}

Value ClientAPI::GetObjectValue(std::string_view flag_key, Value default_value,
                                const EvaluationContext& ctx) {
  return EvaluateObjectFlag(flag_key, default_value, ctx, std::nullopt)
      ->GetValue();
}

Value ClientAPI::GetObjectValue(std::string_view flag_key, Value default_value,
                                const EvaluationOptions& options) {
  return EvaluateObjectFlag(flag_key, default_value, std::nullopt, options)
      ->GetValue();
}

Value ClientAPI::GetObjectValue(std::string_view flag_key, Value default_value,
                                const EvaluationContext& ctx,
                                const EvaluationOptions& options) {
  return EvaluateObjectFlag(flag_key, default_value, ctx, options)->GetValue();
}

ObjectFlagEvaluationDetails ClientAPI::GetObjectDetails(
    std::string_view flag_key, Value default_value) {
  auto res =
      EvaluateObjectFlag(flag_key, default_value, std::nullopt, std::nullopt);
  return ObjectFlagEvaluationDetails(std::string(flag_key), *res);
}

ObjectFlagEvaluationDetails ClientAPI::GetObjectDetails(
    std::string_view flag_key, Value default_value,
    const EvaluationContext& ctx) {
  auto res = EvaluateObjectFlag(flag_key, default_value, ctx, std::nullopt);
  return ObjectFlagEvaluationDetails(std::string(flag_key), *res);
}

ObjectFlagEvaluationDetails ClientAPI::GetObjectDetails(
    std::string_view flag_key, Value default_value,
    const EvaluationOptions& options) {
  auto res = EvaluateObjectFlag(flag_key, default_value, std::nullopt, options);
  return ObjectFlagEvaluationDetails(std::string(flag_key), *res);
}

ObjectFlagEvaluationDetails ClientAPI::GetObjectDetails(
    std::string_view flag_key, Value default_value,
    const EvaluationContext& ctx, const EvaluationOptions& options) {
  auto res = EvaluateObjectFlag(flag_key, default_value, ctx, options);
  return ObjectFlagEvaluationDetails(std::string(flag_key), *res);
}

std::unique_ptr<BoolResolutionDetails> ClientAPI::EvaluateBooleanFlag(
    std::string_view flag_key, bool default_value,
    const std::optional<EvaluationContext>& ctx,
    const std::optional<EvaluationOptions>& options) {
  return this->EvaluateFlag<BoolResolutionDetails>(
      default_value, ctx, options,
      [&](const std::shared_ptr<FeatureProvider>& provider,
          const EvaluationContext& merged_ctx) {
        return provider->GetBooleanEvaluation(flag_key, default_value,
                                              merged_ctx);
      });
}

std::unique_ptr<StringResolutionDetails> ClientAPI::EvaluateStringFlag(
    std::string_view flag_key, std::string_view default_value,
    const std::optional<EvaluationContext>& ctx,
    const std::optional<EvaluationOptions>& options) {
  std::string default_str(default_value);
  return this->EvaluateFlag<StringResolutionDetails>(
      default_str, ctx, options,
      [&](const std::shared_ptr<FeatureProvider>& provider,
          const EvaluationContext& merged_ctx) {
        return provider->GetStringEvaluation(flag_key, default_value,
                                             merged_ctx);
      });
}

std::unique_ptr<IntResolutionDetails> ClientAPI::EvaluateIntegerFlag(
    std::string_view flag_key, int64_t default_value,
    const std::optional<EvaluationContext>& ctx,
    const std::optional<EvaluationOptions>& options) {
  return this->EvaluateFlag<IntResolutionDetails>(
      default_value, ctx, options,
      [&](const std::shared_ptr<FeatureProvider>& provider,
          const EvaluationContext& merged_ctx) {
        return provider->GetIntegerEvaluation(flag_key, default_value,
                                              merged_ctx);
      });
}

std::unique_ptr<DoubleResolutionDetails> ClientAPI::EvaluateDoubleFlag(
    std::string_view flag_key, double default_value,
    const std::optional<EvaluationContext>& ctx,
    const std::optional<EvaluationOptions>& options) {
  return this->EvaluateFlag<DoubleResolutionDetails>(
      default_value, ctx, options,
      [&](const std::shared_ptr<FeatureProvider>& provider,
          const EvaluationContext& merged_ctx) {
        return provider->GetDoubleEvaluation(flag_key, default_value,
                                             merged_ctx);
      });
}

std::unique_ptr<ObjectResolutionDetails> ClientAPI::EvaluateObjectFlag(
    std::string_view flag_key, Value default_value,
    const std::optional<EvaluationContext>& ctx,
    const std::optional<EvaluationOptions>& options) {
  return this->EvaluateFlag<ObjectResolutionDetails>(
      default_value, ctx, options,
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
  }
  return EvaluationContext::Merge({&global_ctx, &client_ctx});
}

void ClientAPI::AddHooks(std::vector<std::shared_ptr<GeneralHook>> hooks) {
  std::unique_lock lock(hooks_mutex_);
  hooks_.reserve(hooks_.size() + hooks.size());
  for (auto& hook : hooks) {
    if (hook != nullptr) {
      hooks_.push_back(std::move(hook));
    }
  }
}

void ClientAPI::AddHook(std::shared_ptr<GeneralHook> hook) {
  if (hook == nullptr) return;
  std::unique_lock lock(hooks_mutex_);
  hooks_.push_back(std::move(hook));
}

std::vector<std::shared_ptr<GeneralHook>> ClientAPI::GetHooks() const {
  std::shared_lock lock(hooks_mutex_);
  return hooks_;
}

}  // namespace openfeature
