#ifndef CPP_SDK_INCLUDE_OPENFEATURE_FEATURE_PROVIDER_STATUS_MANAGER_H_
#define CPP_SDK_INCLUDE_OPENFEATURE_FEATURE_PROVIDER_STATUS_MANAGER_H_

#include <memory>
#include <mutex>

#include "absl/status/statusor.h"
#include "openfeature/evaluation_context.h"
#include "openfeature/provider.h"
#include "openfeature/provider_status.h"

namespace openfeature {

// FeatureProviderStatusManager is a thread-safe wrapper around a
// FeatureProvider that manages its lifecycle and current status.
class FeatureProviderStatusManager {
 public:
  // Factory method replaces the constructor to handle validation errors
  // without throwing exceptions.
  static absl::StatusOr<std::unique_ptr<FeatureProviderStatusManager>> Create(
      std::shared_ptr<FeatureProvider> provider);
  FeatureProviderStatusManager(const FeatureProviderStatusManager&) = delete;
  FeatureProviderStatusManager& operator=(const FeatureProviderStatusManager&) =
      delete;
  ~FeatureProviderStatusManager() = default;

  // Initializes the provider.
  void Init(const EvaluationContext& ctx);

  // Shuts down the provider.
  void Shutdown();

  // Sets the current status of the provider.
  void SetStatus(ProviderStatus status);

  // Gets the current status of the provider.
  ProviderStatus GetStatus() const;

  // Returns the underlying feature provider.
  std::shared_ptr<FeatureProvider> GetProvider() const;

 private:
  explicit FeatureProviderStatusManager(
      std::shared_ptr<FeatureProvider> provider);

  std::shared_ptr<FeatureProvider> provider_;

  mutable std::mutex status_mutex_;
  ProviderStatus status_;
};

}  // namespace openfeature

#endif  // CPP_SDK_INCLUDE_OPENFEATURE_FEATURE_PROVIDER_STATUS_MANAGER_H_
