#ifndef CPP_SDK_INCLUDE_OPENFEATURE_CLIENT_API_H_
#define CPP_SDK_INCLUDE_OPENFEATURE_CLIENT_API_H_

#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>

#include "openfeature/client.h"
#include "openfeature/evaluation_context.h"
#include "openfeature/features.h"
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

  // Evaluate a string flag.
  std::string GetStringValue(std::string_view flag_key,
                             std::string_view default_value) override;
  std::string GetStringValue(std::string_view flag_key,
                             std::string_view default_value,
                             const EvaluationContext& ctx) override;

  // Evaluate an integer flag.
  int64_t GetIntegerValue(std::string_view flag_key,
                          int64_t default_value) override;
  int64_t GetIntegerValue(std::string_view flag_key, int64_t default_value,
                          const EvaluationContext& ctx) override;
  // Evaluate a double flag.
  double GetDoubleValue(std::string_view flag_key,
                        double default_value) override;
  double GetDoubleValue(std::string_view flag_key, double default_value,
                        const EvaluationContext& ctx) override;
  // Evaluate an object flag.
  Value GetObjectValue(std::string_view flag_key, Value default_value) override;
  Value GetObjectValue(std::string_view flag_key, Value default_value,
                       const EvaluationContext& ctx) override;

  // TODO: Add methods to get and set Hooks.
  // TODO: Add methods for detailed flag evaluation.
  // TODO: Overload method "GetBooleanValue" to accept "Evaluation Options".

 private:
  template <typename ResolutionDetailsType, typename ValueType,
            typename ProviderCallable>
  std::unique_ptr<ResolutionDetailsType> EvaluateFlag(
      ValueType default_value, const std::optional<EvaluationContext>& ctx,
      ProviderCallable provider_call);

  std::unique_ptr<BoolResolutionDetails> EvaluateBooleanFlag(
      std::string_view flag_key, bool default_value,
      const std::optional<EvaluationContext>& ctx);

  std::unique_ptr<StringResolutionDetails> EvaluateStringFlag(
      std::string_view flag_key, std::string_view default_value,
      const std::optional<EvaluationContext>& ctx);

  std::unique_ptr<IntResolutionDetails> EvaluateIntegerFlag(
      std::string_view flag_key, int64_t default_value,
      const std::optional<EvaluationContext>& ctx);

  std::unique_ptr<DoubleResolutionDetails> EvaluateDoubleFlag(
      std::string_view flag_key, double default_value,
      const std::optional<EvaluationContext>& ctx);

  std::unique_ptr<ObjectResolutionDetails> EvaluateObjectFlag(
      std::string_view flag_key, Value default_value,
      const std::optional<EvaluationContext>& ctx);

  EvaluationContext MergeContexts(
      const std::optional<EvaluationContext>& invocation_ctx);

  ProviderRepository& provider_repository_;
  std::string domain_;
  EvaluationContext evaluation_context_;
  mutable std::mutex context_mutex_;
};

template <typename ResolutionDetailsType, typename ValueType,
          typename ProviderCallable>
std::unique_ptr<ResolutionDetailsType> ClientAPI::EvaluateFlag(
    ValueType default_value, const std::optional<EvaluationContext>& ctx,
    ProviderCallable provider_call) {
  if (GetProviderStatus() != ProviderStatus::kReady) {
    return std::make_unique<ResolutionDetailsType>(
        default_value, Reason::kError, std::nullopt, FlagMetadata(),
        ErrorCode::kProviderNotReady, "Provider is not ready");
  }

  std::shared_ptr<FeatureProvider> provider =
      provider_repository_.GetProvider(domain_);
  if (!provider) {
    return std::make_unique<ResolutionDetailsType>(
        default_value, Reason::kError, std::nullopt, FlagMetadata(),
        ErrorCode::kProviderFatal, "Provider not found for domain");
  }

  EvaluationContext merged_context = MergeContexts(ctx);
  return provider_call(provider, merged_context);
}

}  // namespace openfeature

#endif  // CPP_SDK_INCLUDE_OPENFEATURE_CLIENT_API_H_
