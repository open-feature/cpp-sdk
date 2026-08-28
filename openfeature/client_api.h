#ifndef CPP_SDK_INCLUDE_OPENFEATURE_CLIENT_API_H_
#define CPP_SDK_INCLUDE_OPENFEATURE_CLIENT_API_H_

#include <algorithm>
#include <memory>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

#include "openfeature/client.h"
#include "openfeature/evaluation_context.h"
#include "openfeature/evaluation_options.h"
#include "openfeature/features.h"
#include "openfeature/flag_evaluation_details.h"
#include "openfeature/flag_type_value.h"
#include "openfeature/general_hook.h"
#include "openfeature/global_context_manager.h"
#include "openfeature/global_hook_manager.h"
#include "openfeature/hook_context.h"
#include "openfeature/hook_data.h"
#include "openfeature/hook_hints.h"
#include "openfeature/hook_support.h"
#include "openfeature/metadata.h"
#include "openfeature/provider.h"
#include "openfeature/provider_repository.h"
#include "openfeature/provider_status.h"
#include "openfeature/resolution_details.h"
#include "openfeature/value.h"

namespace openfeature {

// OpenFeature client implementation.
class ClientAPI : public Client {
 public:
  ClientAPI(ProviderRepository& repository, std::string_view domain);

  ~ClientAPI() override = default;

  ClientAPI(const ClientAPI&) = delete;
  ClientAPI& operator=(const ClientAPI&) = delete;

  Metadata GetMetadata() override;

  // Return an optional client-level evaluation context.
  EvaluationContext GetEvaluationContext() override;

  // Set the client-level evaluation context.
  void SetEvaluationContext(const EvaluationContext& ctx) override;

  // Returns the current status of the associated provider.
  ProviderStatus GetProviderStatus() override;

  // Evaluate a boolean flag.
  bool GetBooleanValue(std::string_view flag_key, bool default_value) override;
  bool GetBooleanValue(std::string_view flag_key, bool default_value,
                       const EvaluationContext& ctx) override;
  bool GetBooleanValue(std::string_view flag_key, bool default_value,
                       const EvaluationOptions& options) override;
  bool GetBooleanValue(std::string_view flag_key, bool default_value,
                       const EvaluationContext& ctx,
                       const EvaluationOptions& options) override;

  // Detailed boolean flag evaluation.
  BoolFlagEvaluationDetails GetBooleanDetails(std::string_view flag_key,
                                              bool default_value) override;
  BoolFlagEvaluationDetails GetBooleanDetails(
      std::string_view flag_key, bool default_value,
      const EvaluationContext& ctx) override;
  BoolFlagEvaluationDetails GetBooleanDetails(
      std::string_view flag_key, bool default_value,
      const EvaluationOptions& options) override;
  BoolFlagEvaluationDetails GetBooleanDetails(
      std::string_view flag_key, bool default_value,
      const EvaluationContext& ctx, const EvaluationOptions& options) override;

  // Evaluate a string flag.
  std::string GetStringValue(std::string_view flag_key,
                             std::string_view default_value) override;
  std::string GetStringValue(std::string_view flag_key,
                             std::string_view default_value,
                             const EvaluationContext& ctx) override;
  std::string GetStringValue(std::string_view flag_key,
                             std::string_view default_value,
                             const EvaluationOptions& options) override;
  std::string GetStringValue(std::string_view flag_key,
                             std::string_view default_value,
                             const EvaluationContext& ctx,
                             const EvaluationOptions& options) override;

  // Detailed string flag evaluation.
  StringFlagEvaluationDetails GetStringDetails(
      std::string_view flag_key, std::string_view default_value) override;
  StringFlagEvaluationDetails GetStringDetails(
      std::string_view flag_key, std::string_view default_value,
      const EvaluationContext& ctx) override;
  StringFlagEvaluationDetails GetStringDetails(
      std::string_view flag_key, std::string_view default_value,
      const EvaluationOptions& options) override;
  StringFlagEvaluationDetails GetStringDetails(
      std::string_view flag_key, std::string_view default_value,
      const EvaluationContext& ctx, const EvaluationOptions& options) override;

  // Evaluate an integer flag.
  int64_t GetIntegerValue(std::string_view flag_key,
                          int64_t default_value) override;
  int64_t GetIntegerValue(std::string_view flag_key, int64_t default_value,
                          const EvaluationContext& ctx) override;
  int64_t GetIntegerValue(std::string_view flag_key, int64_t default_value,
                          const EvaluationOptions& options) override;
  int64_t GetIntegerValue(std::string_view flag_key, int64_t default_value,
                          const EvaluationContext& ctx,
                          const EvaluationOptions& options) override;

  // Detailed integer flag evaluation.
  IntFlagEvaluationDetails GetIntegerDetails(std::string_view flag_key,
                                             int64_t default_value) override;
  IntFlagEvaluationDetails GetIntegerDetails(
      std::string_view flag_key, int64_t default_value,
      const EvaluationContext& ctx) override;
  IntFlagEvaluationDetails GetIntegerDetails(
      std::string_view flag_key, int64_t default_value,
      const EvaluationOptions& options) override;
  IntFlagEvaluationDetails GetIntegerDetails(
      std::string_view flag_key, int64_t default_value,
      const EvaluationContext& ctx, const EvaluationOptions& options) override;

  // Evaluate a double flag.
  double GetDoubleValue(std::string_view flag_key,
                        double default_value) override;
  double GetDoubleValue(std::string_view flag_key, double default_value,
                        const EvaluationContext& ctx) override;
  double GetDoubleValue(std::string_view flag_key, double default_value,
                        const EvaluationOptions& options) override;
  double GetDoubleValue(std::string_view flag_key, double default_value,
                        const EvaluationContext& ctx,
                        const EvaluationOptions& options) override;

  // Detailed double flag evaluation.
  DoubleFlagEvaluationDetails GetDoubleDetails(std::string_view flag_key,
                                               double default_value) override;
  DoubleFlagEvaluationDetails GetDoubleDetails(
      std::string_view flag_key, double default_value,
      const EvaluationContext& ctx) override;
  DoubleFlagEvaluationDetails GetDoubleDetails(
      std::string_view flag_key, double default_value,
      const EvaluationOptions& options) override;
  DoubleFlagEvaluationDetails GetDoubleDetails(
      std::string_view flag_key, double default_value,
      const EvaluationContext& ctx, const EvaluationOptions& options) override;

  // Evaluate an object flag.
  Value GetObjectValue(std::string_view flag_key, Value default_value) override;
  Value GetObjectValue(std::string_view flag_key, Value default_value,
                       const EvaluationContext& ctx) override;
  Value GetObjectValue(std::string_view flag_key, Value default_value,
                       const EvaluationOptions& options) override;
  Value GetObjectValue(std::string_view flag_key, Value default_value,
                       const EvaluationContext& ctx,
                       const EvaluationOptions& options) override;

  // Detailed object flag evaluation.
  ObjectFlagEvaluationDetails GetObjectDetails(std::string_view flag_key,
                                               Value default_value) override;
  ObjectFlagEvaluationDetails GetObjectDetails(
      std::string_view flag_key, Value default_value,
      const EvaluationContext& ctx) override;
  ObjectFlagEvaluationDetails GetObjectDetails(
      std::string_view flag_key, Value default_value,
      const EvaluationOptions& options) override;
  ObjectFlagEvaluationDetails GetObjectDetails(
      std::string_view flag_key, Value default_value,
      const EvaluationContext& ctx, const EvaluationOptions& options) override;

  // Adds one or more hooks to the client-level hook repository.
  void AddHooks(std::vector<std::shared_ptr<GeneralHook>> hooks) override;

  // Adds a single hook to the client-level hook repository.
  void AddHook(std::shared_ptr<GeneralHook> hook) override;

  // Retrieves all configured client-level hooks.
  std::vector<std::shared_ptr<GeneralHook>> GetHooks() const override;

 private:
  template <typename ResolutionDetailsType, typename ValueType,
            typename ProviderCallable>
  std::unique_ptr<ResolutionDetailsType> EvaluateFlag(
      std::string_view flag_key, FlagValueType flag_type,
      ValueType default_value, const std::optional<EvaluationContext>& ctx,
      const std::optional<EvaluationOptions>& options,
      ProviderCallable provider_call);

  std::unique_ptr<BoolResolutionDetails> EvaluateBooleanFlag(
      std::string_view flag_key, bool default_value,
      const std::optional<EvaluationContext>& ctx,
      const std::optional<EvaluationOptions>& options = std::nullopt);

  std::unique_ptr<StringResolutionDetails> EvaluateStringFlag(
      std::string_view flag_key, std::string_view default_value,
      const std::optional<EvaluationContext>& ctx,
      const std::optional<EvaluationOptions>& options = std::nullopt);

  std::unique_ptr<IntResolutionDetails> EvaluateIntegerFlag(
      std::string_view flag_key, int64_t default_value,
      const std::optional<EvaluationContext>& ctx,
      const std::optional<EvaluationOptions>& options = std::nullopt);

  std::unique_ptr<DoubleResolutionDetails> EvaluateDoubleFlag(
      std::string_view flag_key, double default_value,
      const std::optional<EvaluationContext>& ctx,
      const std::optional<EvaluationOptions>& options = std::nullopt);

  std::unique_ptr<ObjectResolutionDetails> EvaluateObjectFlag(
      std::string_view flag_key, Value default_value,
      const std::optional<EvaluationContext>& ctx,
      const std::optional<EvaluationOptions>& options = std::nullopt);

  EvaluationContext MergeContexts(
      const std::optional<EvaluationContext>& invocation_ctx);

  template <typename ValueType, typename ProviderCallable>
  void ResolveProvider(
      const std::shared_ptr<FeatureProvider>& provider,
      const std::shared_ptr<FeatureProviderStatusManager>& manager,
      ProviderStatus provider_status, const EvaluationContext& merged_context,
      std::string_view flag_key, ProviderCallable& provider_call,
      std::unique_ptr<FlagEvaluationDetails<ValueType>>& evaluation_details,
      std::optional<ErrorCode>& error_code, std::string& error_message,
      std::unique_ptr<std::exception>& captured_exception,
      bool& has_error) const;

  ProviderRepository& provider_repository_;
  std::string domain_;
  EvaluationContext evaluation_context_;
  mutable std::mutex context_mutex_;
  mutable std::shared_mutex hooks_mutex_;
  std::vector<std::shared_ptr<GeneralHook>> hooks_;
};

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
    captured_exception = std::make_unique<std::runtime_error>(error_message);
  } else if (provider_status == ProviderStatus::kNotReady) {
    has_error = true;
    error_code = ErrorCode::kProviderNotReady;
    error_message = "Provider is not ready";
    captured_exception = std::make_unique<std::runtime_error>(error_message);
  } else if (provider_status == ProviderStatus::kFatal) {
    has_error = true;
    error_code = ErrorCode::kProviderFatal;
    error_message = "Provider is in fatal error state";
    captured_exception = std::make_unique<std::runtime_error>(error_message);
  } else if (!provider) {
    has_error = true;
    error_code = ErrorCode::kProviderFatal;
    error_message = "Provider not found for domain";
    captured_exception = std::make_unique<std::runtime_error>(error_message);
  } else {
    try {
      auto result = provider_call(provider, merged_context);
      if (!result.ok()) {
        has_error = true;
        error_code = ErrorCode::kGeneral;
        error_message = std::string(result.status().message());
        captured_exception =
            std::make_unique<std::runtime_error>(error_message);
      } else if (*result == nullptr) {
        has_error = true;
        error_code = ErrorCode::kGeneral;
        error_message = "Provider returned null resolution details";
        captured_exception =
            std::make_unique<std::runtime_error>(error_message);
      } else if ((*result)->GetErrorCode().has_value()) {
        has_error = true;
        error_code = (*result)->GetErrorCode();
        error_message = (*result)->GetErrorMessage().value_or("Provider error");
        captured_exception =
            std::make_unique<std::runtime_error>(error_message);
      } else {
        evaluation_details = std::make_unique<FlagEvaluationDetails<ValueType>>(
            std::string(flag_key), **result);
      }
    } catch (const std::exception& exception) {
      has_error = true;
      error_code = ErrorCode::kGeneral;
      error_message =
          std::string("Exception during evaluation: ") + exception.what();
      captured_exception =
          std::make_unique<std::runtime_error>(exception.what());
    } catch (...) {
      has_error = true;
      error_code = ErrorCode::kGeneral;
      error_message = "Unknown exception during evaluation";
      captured_exception = std::make_unique<std::runtime_error>(error_message);
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
  bool is_provider_ready =
      (manager != nullptr && provider_status != ProviderStatus::kNotReady &&
       provider_status != ProviderStatus::kFatal);
  std::shared_ptr<FeatureProvider> provider =
      (is_provider_ready && manager) ? manager->GetProvider() : nullptr;

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

#endif  // CPP_SDK_INCLUDE_OPENFEATURE_CLIENT_API_H_
