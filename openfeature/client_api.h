#ifndef CPP_SDK_INCLUDE_OPENFEATURE_CLIENT_API_H_
#define CPP_SDK_INCLUDE_OPENFEATURE_CLIENT_API_H_

#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "openfeature/client.h"
#include "openfeature/evaluation_context.h"
#include "openfeature/features.h"
#include "openfeature/general_hook.h"
#include "openfeature/global_context_manager.h"
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

  // Evaluate an object flag.
  Value GetObjectValue(std::string_view flag_key, Value default_value) override;
  Value GetObjectValue(std::string_view flag_key, Value default_value,
                       const EvaluationContext& ctx) override;
  Value GetObjectValue(std::string_view flag_key, Value default_value,
                       const EvaluationOptions& options) override;
  Value GetObjectValue(std::string_view flag_key, Value default_value,
                       const EvaluationContext& ctx,
                       const EvaluationOptions& options) override;

  // Adds one or more hooks to the client-level hook repository.
  void AddHooks(std::vector<std::shared_ptr<GeneralHook>> hooks) override;

  // Adds a single hook to the client-level hook repository.
  void AddHook(std::shared_ptr<GeneralHook> hook) override;

  // Retrieves all configured client-level hooks.
  std::vector<std::shared_ptr<GeneralHook>> GetHooks() const override;

  // TODO: Add methods for detailed flag evaluation.

 private:
  template <typename ResolutionDetailsType, typename ValueType,
            typename ProviderCallable>
  std::unique_ptr<ResolutionDetailsType> EvaluateFlag(
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

  ProviderRepository& provider_repository_;
  std::string domain_;
  EvaluationContext evaluation_context_;
  mutable std::mutex context_mutex_;
  mutable std::shared_mutex hooks_mutex_;
  std::vector<std::shared_ptr<GeneralHook>> hooks_;
};

template <typename ResolutionDetailsType, typename ValueType,
          typename ProviderCallable>
std::unique_ptr<ResolutionDetailsType> ClientAPI::EvaluateFlag(
    ValueType default_value, const std::optional<EvaluationContext>& ctx,
    const std::optional<EvaluationOptions>& options,
    ProviderCallable provider_call) {
  std::shared_ptr<FeatureProviderStatusManager> manager =
      provider_repository_.GetFeatureProviderStatusManager(domain_);
  if (!manager) {
    return std::make_unique<ResolutionDetailsType>(
        default_value, Reason::kError, std::nullopt, FlagMetadata(),
        ErrorCode::kGeneral, "Provider status manager not found for domain");
  }

  ProviderStatus status = manager->GetStatus();
  if (status == ProviderStatus::kNotReady) {
    return std::make_unique<ResolutionDetailsType>(
        default_value, Reason::kError, std::nullopt, FlagMetadata(),
        ErrorCode::kProviderNotReady, "Provider is not ready");
  }
  if (status == ProviderStatus::kFatal) {
    return std::make_unique<ResolutionDetailsType>(
        default_value, Reason::kError, std::nullopt, FlagMetadata(),
        ErrorCode::kProviderFatal, "Provider is in fatal error state");
  }

  std::shared_ptr<FeatureProvider> provider = manager->GetProvider();
  if (!provider) {
    return std::make_unique<ResolutionDetailsType>(
        default_value, Reason::kError, std::nullopt, FlagMetadata(),
        ErrorCode::kProviderFatal, "Provider not found for domain");
  }

  EvaluationContext merged_context = MergeContexts(ctx);

  try {
    auto result = provider_call(provider, merged_context);

    if (!result.ok()) {
      return std::make_unique<ResolutionDetailsType>(
          default_value, Reason::kError, std::nullopt, FlagMetadata(),
          ErrorCode::kGeneral, std::string(result.status().message()));
    }
    if (*result == nullptr) {
      return std::make_unique<ResolutionDetailsType>(
          default_value, Reason::kError, std::nullopt, FlagMetadata(),
          ErrorCode::kGeneral, "Provider returned null resolution details");
    }
    return std::move(*result);
  } catch (const std::exception& e) {
    return std::make_unique<ResolutionDetailsType>(
        default_value, Reason::kError, std::nullopt, FlagMetadata(),
        ErrorCode::kGeneral,
        std::string("Exception during evaluation: ") + e.what());
  } catch (...) {
    return std::make_unique<ResolutionDetailsType>(
        default_value, Reason::kError, std::nullopt, FlagMetadata(),
        ErrorCode::kGeneral, "Unknown exception during evaluation");
  }
}
}  // namespace openfeature

#endif  // CPP_SDK_INCLUDE_OPENFEATURE_CLIENT_API_H_
