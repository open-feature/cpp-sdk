#include "client_api.h"

#include <utility>

#include "openfeature/exceptions/open_feature_exceptions.h"
#include "openfeature/flag_metadata.h"
#include "openfeature/flag_type_value.h"
#include "openfeature/global_context_manager.h"
#include "openfeature/reason.h"

namespace openfeature {

ClientAPI::ClientAPI(ProviderRepository& repository, std::string_view domain)
    : provider_repository_(repository),
      domain_(domain),
      evaluation_context_(EvaluationContext::Builder().Build()) {}

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
  return {std::string(flag_key), *res};
}

BoolFlagEvaluationDetails ClientAPI::GetBooleanDetails(
    std::string_view flag_key, bool default_value,
    const EvaluationContext& ctx) {
  auto res = EvaluateBooleanFlag(flag_key, default_value, ctx, std::nullopt);
  return {std::string(flag_key), *res};
}

BoolFlagEvaluationDetails ClientAPI::GetBooleanDetails(
    std::string_view flag_key, bool default_value,
    const EvaluationOptions& options) {
  auto res =
      EvaluateBooleanFlag(flag_key, default_value, std::nullopt, options);
  return {std::string(flag_key), *res};
}

BoolFlagEvaluationDetails ClientAPI::GetBooleanDetails(
    std::string_view flag_key, bool default_value, const EvaluationContext& ctx,
    const EvaluationOptions& options) {
  auto res = EvaluateBooleanFlag(flag_key, default_value, ctx, options);
  return {std::string(flag_key), *res};
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
  return {std::string(flag_key), *res};
}

StringFlagEvaluationDetails ClientAPI::GetStringDetails(
    std::string_view flag_key, std::string_view default_value,
    const EvaluationContext& ctx) {
  auto res = EvaluateStringFlag(flag_key, default_value, ctx, std::nullopt);
  return {std::string(flag_key), *res};
}

StringFlagEvaluationDetails ClientAPI::GetStringDetails(
    std::string_view flag_key, std::string_view default_value,
    const EvaluationOptions& options) {
  auto res = EvaluateStringFlag(flag_key, default_value, std::nullopt, options);
  return {std::string(flag_key), *res};
}

StringFlagEvaluationDetails ClientAPI::GetStringDetails(
    std::string_view flag_key, std::string_view default_value,
    const EvaluationContext& ctx, const EvaluationOptions& options) {
  auto res = EvaluateStringFlag(flag_key, default_value, ctx, options);
  return {std::string(flag_key), *res};
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
  return {std::string(flag_key), *res};
}

IntFlagEvaluationDetails ClientAPI::GetIntegerDetails(
    std::string_view flag_key, int64_t default_value,
    const EvaluationContext& ctx) {
  auto res = EvaluateIntegerFlag(flag_key, default_value, ctx, std::nullopt);
  return {std::string(flag_key), *res};
}

IntFlagEvaluationDetails ClientAPI::GetIntegerDetails(
    std::string_view flag_key, int64_t default_value,
    const EvaluationOptions& options) {
  auto res =
      EvaluateIntegerFlag(flag_key, default_value, std::nullopt, options);
  return {std::string(flag_key), *res};
}

IntFlagEvaluationDetails ClientAPI::GetIntegerDetails(
    std::string_view flag_key, int64_t default_value,
    const EvaluationContext& ctx, const EvaluationOptions& options) {
  auto res = EvaluateIntegerFlag(flag_key, default_value, ctx, options);
  return {std::string(flag_key), *res};
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
  return {std::string(flag_key), *res};
}

DoubleFlagEvaluationDetails ClientAPI::GetDoubleDetails(
    std::string_view flag_key, double default_value,
    const EvaluationContext& ctx) {
  auto res = EvaluateDoubleFlag(flag_key, default_value, ctx, std::nullopt);
  return {std::string(flag_key), *res};
}

DoubleFlagEvaluationDetails ClientAPI::GetDoubleDetails(
    std::string_view flag_key, double default_value,
    const EvaluationOptions& options) {
  auto res = EvaluateDoubleFlag(flag_key, default_value, std::nullopt, options);
  return {std::string(flag_key), *res};
}

DoubleFlagEvaluationDetails ClientAPI::GetDoubleDetails(
    std::string_view flag_key, double default_value,
    const EvaluationContext& ctx, const EvaluationOptions& options) {
  auto res = EvaluateDoubleFlag(flag_key, default_value, ctx, options);
  return {std::string(flag_key), *res};
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
  return {std::string(flag_key), *res};
}

ObjectFlagEvaluationDetails ClientAPI::GetObjectDetails(
    std::string_view flag_key, Value default_value,
    const EvaluationContext& ctx) {
  auto res = EvaluateObjectFlag(flag_key, default_value, ctx, std::nullopt);
  return {std::string(flag_key), *res};
}

ObjectFlagEvaluationDetails ClientAPI::GetObjectDetails(
    std::string_view flag_key, Value default_value,
    const EvaluationOptions& options) {
  auto res = EvaluateObjectFlag(flag_key, default_value, std::nullopt, options);
  return {std::string(flag_key), *res};
}

ObjectFlagEvaluationDetails ClientAPI::GetObjectDetails(
    std::string_view flag_key, Value default_value,
    const EvaluationContext& ctx, const EvaluationOptions& options) {
  auto res = EvaluateObjectFlag(flag_key, default_value, ctx, options);
  return {std::string(flag_key), *res};
}

std::unique_ptr<BoolResolutionDetails> ClientAPI::EvaluateBooleanFlag(
    std::string_view flag_key, bool default_value,
    const std::optional<EvaluationContext>& ctx,
    const std::optional<EvaluationOptions>& options) {
  return this->EvaluateFlag<BoolResolutionDetails>(
      flag_key, FlagValueType::kBoolean, default_value, ctx, options,
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
      flag_key, FlagValueType::kString, default_str, ctx, options,
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
      flag_key, FlagValueType::kInteger, default_value, ctx, options,
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
      flag_key, FlagValueType::kDouble, default_value, ctx, options,
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
      flag_key, FlagValueType::kObject, default_value, ctx, options,
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

template <typename ValueType, typename ProviderCallable>
void ClientAPI::ResolveProvider(
    const std::shared_ptr<FeatureProvider>& provider,
    const std::shared_ptr<FeatureProviderStatusManager>& manager,
    ProviderStatus provider_status, const EvaluationContext& merged_context,
    std::string_view flag_key, ProviderCallable& provider_call,
    std::unique_ptr<FlagEvaluationDetails<ValueType>>& evaluation_details,
    std::optional<ErrorCode>& error_code, std::string& error_message,
    std::unique_ptr<std::exception>& captured_exception,
    bool& has_error) const {
  if (!manager) {
    has_error = true;
    error_code = ErrorCode::kGeneral;
    error_message = "Provider status manager not found for domain";
    captured_exception = std::make_unique<OpenFeatureException>(
        error_code.value_or(ErrorCode::kGeneral), error_message);
  } else if (provider_status == ProviderStatus::kNotReady) {
    has_error = true;
    error_code = ErrorCode::kProviderNotReady;
    error_message = "Provider is not ready";
    captured_exception = std::make_unique<OpenFeatureException>(
        error_code.value_or(ErrorCode::kGeneral), error_message);
  } else if (provider_status == ProviderStatus::kFatal) {
    has_error = true;
    error_code = ErrorCode::kProviderFatal;
    error_message = "Provider is in fatal error state";
    captured_exception = std::make_unique<OpenFeatureException>(
        error_code.value_or(ErrorCode::kGeneral), error_message);
  } else if (!provider) {
    has_error = true;
    error_code = ErrorCode::kProviderFatal;
    error_message = "Provider not found for domain";
    captured_exception = std::make_unique<OpenFeatureException>(
        error_code.value_or(ErrorCode::kGeneral), error_message);
  } else {
    try {
      auto result = provider_call(provider, merged_context);
      if (!result.ok()) {
        has_error = true;
        error_code = ErrorCode::kGeneral;
        error_message = std::string(result.status().message());
        captured_exception = std::make_unique<OpenFeatureException>(
            error_code.value_or(ErrorCode::kGeneral), error_message);
      } else if (*result == nullptr) {
        has_error = true;
        error_code = ErrorCode::kGeneral;
        error_message = "Provider returned null resolution details";
        captured_exception = std::make_unique<OpenFeatureException>(
            error_code.value_or(ErrorCode::kGeneral), error_message);
      } else {
        evaluation_details = std::make_unique<FlagEvaluationDetails<ValueType>>(
            std::string(flag_key), **result);

        if ((*result)->GetErrorCode().has_value()) {
          has_error = true;
          error_code = (*result)->GetErrorCode();
          error_message =
              (*result)->GetErrorMessage().value_or("Provider error");
          captured_exception = std::make_unique<OpenFeatureException>(
              error_code.value_or(ErrorCode::kGeneral), error_message);
        }
      }
    } catch (const std::exception& exception) {
      has_error = true;
      error_code = ErrorCode::kGeneral;
      error_message =
          std::string("Exception during evaluation: ") + exception.what();
      captured_exception = std::make_unique<OpenFeatureException>(
          error_code.value_or(ErrorCode::kGeneral), error_message);
    } catch (...) {
      has_error = true;
      error_code = ErrorCode::kGeneral;
      error_message = "Unknown exception during evaluation";
      captured_exception = std::make_unique<OpenFeatureException>(
          error_code.value_or(ErrorCode::kGeneral), error_message);
    }
  }
}

template <typename ResolutionDetailsType, typename ValueType,
          typename ProviderCallable>
std::unique_ptr<ResolutionDetailsType> ClientAPI::EvaluateFlag(
    std::string_view flag_key, FlagValueType flag_type, ValueType default_value,
    const std::optional<EvaluationContext>& ctx,
    const std::optional<EvaluationOptions>& options,
    ProviderCallable provider_call) {
  std::shared_ptr<FeatureProviderStatusManager> manager =
      provider_repository_.GetFeatureProviderStatusManager(domain_);
  ProviderStatus provider_status =
      manager ? manager->GetStatus() : ProviderStatus::kNotReady;
  std::shared_ptr<FeatureProvider> provider =
      manager ? manager->GetProvider() : nullptr;

  // Collect hooks in order of increasing specificity
  std::vector<std::shared_ptr<GeneralHook>> forward_hooks =
      HookSupport::CollectHooks(GetHooks(), options, provider);

  // Reverse list for after, error, and finally stages
  std::vector<std::shared_ptr<GeneralHook>> reverse_hooks(
      forward_hooks.rbegin(), forward_hooks.rend());

  HookHints hints = options.has_value() ? options->hook_hints : HookHints{};
  auto hook_data_map = HookSupport::CreateHookDataMap(forward_hooks);

  Metadata client_metadata = GetMetadata();
  Metadata provider_metadata =
      provider ? provider->GetMetadata() : Metadata{""};

  // Initialize merged context: Global -> Client -> Invocation
  EvaluationContext merged_context = MergeContexts(ctx);

  bool has_error = false;
  std::string error_message;
  std::optional<ErrorCode> error_code = std::nullopt;
  std::unique_ptr<std::exception> captured_exception;
  std::unique_ptr<FlagEvaluationDetails<ValueType>> evaluation_details;

  // Before Stage
  if (!HookSupport::ExecuteBeforeHooks(
          forward_hooks, flag_key, flag_type, default_value, client_metadata,
          provider_metadata, hints, hook_data_map, merged_context, error_code,
          error_message, captured_exception)) {
    has_error = true;
  }

  // Provider Resolution stage (only if no error in Before)
  if (!has_error) {
    ResolveProvider(provider, manager, provider_status, merged_context,
                    flag_key, provider_call, evaluation_details, error_code,
                    error_message, captured_exception, has_error);
  }

  // Construct error evaluation_details if error occurred in Before or
  // Resolution
  if (has_error && !evaluation_details) {
    evaluation_details = std::make_unique<FlagEvaluationDetails<ValueType>>(
        std::string(flag_key), default_value, Reason::kError, std::nullopt,
        FlagMetadata(), error_code.value_or(ErrorCode::kGeneral),
        error_message);
  }

  // After stage (only if no error occurred)
  if (!has_error && evaluation_details) {
    HookSupport::ExecuteAfterHooks(
        reverse_hooks, flag_key, flag_type, default_value, merged_context,
        client_metadata, provider_metadata, hints, hook_data_map,
        evaluation_details, error_code, error_message, captured_exception,
        has_error);
  }

  // Error stage
  if (has_error && captured_exception) {
    HookSupport::ExecuteErrorHooks(reverse_hooks, flag_key, flag_type,
                                   default_value, merged_context,
                                   client_metadata, provider_metadata, hints,
                                   hook_data_map, *captured_exception);
  }

  // Finally stage (always executed)
  if (!evaluation_details) {
    evaluation_details = std::make_unique<FlagEvaluationDetails<ValueType>>(
        std::string(flag_key), default_value, Reason::kError, std::nullopt,
        FlagMetadata(), error_code.value_or(ErrorCode::kGeneral),
        error_message);
  }

  HookSupport::ExecuteFinallyHooks(reverse_hooks, flag_key, flag_type,
                                   default_value, merged_context,
                                   client_metadata, provider_metadata, hints,
                                   hook_data_map, *evaluation_details);

  return std::make_unique<ResolutionDetailsType>(
      evaluation_details->GetValue(), evaluation_details->GetReason(),
      evaluation_details->GetVariant(), evaluation_details->GetFlagMetadata(),
      evaluation_details->GetErrorCode(),
      evaluation_details->GetErrorMessage());
}

}  // namespace openfeature
