#ifndef CPP_SDK_INCLUDE_OPENFEATURE_PROVIDER_REPOSITORY_H_
#define CPP_SDK_INCLUDE_OPENFEATURE_PROVIDER_REPOSITORY_H_

#include <memory>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <string>
#include <string_view>
#include <thread>
#include <unordered_map>
#include <vector>

#include "absl/status/status.h"
#include "openfeature/evaluation_context.h"
#include "openfeature/feature_provider_status_manager.h"
#include "openfeature/provider.h"
#include "openfeature/provider_status.h"

namespace openfeature {

// ProviderRepository holds a default provider and allows for registering named
// providers associated with specific domains. The repository is responsible for
// the initialization and shutdown of providers and provides thread-safe access
// to them.
class ProviderRepository {
 public:
  ProviderRepository();
  ~ProviderRepository();

  ProviderRepository(const ProviderRepository&) = delete;
  ProviderRepository& operator=(const ProviderRepository&) = delete;

  // If the domain is empty, fetch status manager for the default provider.
  // Otherwise, fetch the status manager for a specific provider.
  std::shared_ptr<FeatureProviderStatusManager> GetFeatureProviderStatusManager(
      std::string_view domain = "") const;

  // If the domain is empty then GetProvider returns the default
  // FeatureProvider. Otherwise it returns the FeatureProvider for the
  // domain.
  std::shared_ptr<FeatureProvider> GetProvider(
      std::string_view domain = "") const;

  // Set the default provider.
  void SetProvider(std::shared_ptr<FeatureProvider> provider,
                   const EvaluationContext& ctx, bool waitForInit);

  // Add a provider for a domain.
  void SetProvider(std::string_view domain,
                   std::shared_ptr<FeatureProvider> provider,
                   const EvaluationContext& ctx, bool waitForInit);

  // Fetch the status of a provider for a domain.
  // If the domain is not set, return the default provider status.
  // If not found, return the default.
  ProviderStatus GetProviderStatus(std::string_view domain = "") const;

  // Wait for any pending initialization threads to complete before shutting
  // down providers. Afterwards, shuts down all registered providers and
  // clears the repository.
  void Shutdown();

 private:
  void PrepareAndInitializeProvider(
      const std::optional<std::string> domain,
      std::shared_ptr<FeatureProvider> new_provider,
      const EvaluationContext& ctx, bool waitForInit);

  void InitializeProvider(
      std::shared_ptr<FeatureProviderStatusManager> new_status_manager,
      std::shared_ptr<FeatureProviderStatusManager> old_status_manager,
      const EvaluationContext& ctx);

  void ShutdownOldProvider(
      std::shared_ptr<FeatureProviderStatusManager> old_status_manager);

  std::shared_ptr<FeatureProviderStatusManager>
  GetExistingStatusManagerForProvider(
      const std::shared_ptr<FeatureProvider>& provider);

  bool IsStatusManagerRegistered(
      const std::shared_ptr<FeatureProviderStatusManager>& manager);

  std::unordered_map<std::string, std::shared_ptr<FeatureProviderStatusManager>>
      provider_manager_;
  std::shared_ptr<FeatureProviderStatusManager> default_manager_;
  mutable std::shared_mutex repo_mutex_;

  std::vector<std::thread> initialization_threads_;
  std::mutex threads_mutex_;
};

}  // namespace openfeature

#endif  // CPP_SDK_INCLUDE_OPENFEATURE_PROVIDER_REPOSITORY_H_
