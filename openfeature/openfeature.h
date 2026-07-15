#ifndef CPP_SDK_INCLUDE_OPENFEATURE_OPENFEATURE_H_
#define CPP_SDK_INCLUDE_OPENFEATURE_OPENFEATURE_H_

#include <chrono>
#include <memory>
#include <shared_mutex>
#include <string_view>
#include <vector>

#include "openfeature/base_hook.h"
#include "openfeature/client.h"
#include "openfeature/evaluation_context.h"
#include "openfeature/metadata.h"
#include "openfeature/provider.h"

namespace openfeature {

class OpenFeature {
 public:
  virtual ~OpenFeature() = default;

  // Sets the default provider.
  virtual void SetProvider(std::shared_ptr<FeatureProvider> provider) = 0;

  // Sets a provider for a specific domain.
  virtual void SetProvider(std::string_view domain,
                           std::shared_ptr<FeatureProvider> provider) = 0;

  // Sets the default provider and blocks until it successfully initializes.
  virtual void SetProviderAndWait(
      std::shared_ptr<FeatureProvider> provider) = 0;

  // Sets a named provider and blocks until it successfully initializes.
  virtual void SetProviderAndWait(
      std::string_view domain, std::shared_ptr<FeatureProvider> provider) = 0;

  // If the domain is empty then GetProvider returns the default provider
  // otherwise it returns the provider for the domain. If this domain has no
  // provider bound, it returns the default provider.
  virtual std::shared_ptr<FeatureProvider> GetProvider(
      std::string_view domain = "") const = 0;

  virtual std::shared_ptr<Client> GetClient() = 0;

  virtual std::shared_ptr<Client> GetClient(std::string_view domain) = 0;

  // Sets the global evaluation context.
  virtual void SetEvaluationContext(const EvaluationContext& ctx) = 0;

  // Gets the global evaluation context.
  virtual EvaluationContext GetEvaluationContext() const = 0;

  // Gets the metadata for a provider bound to a specific domain.
  virtual Metadata GetProviderMetadata(std::string_view domain = "") const = 0;

  // Fetches the status of a provider for a domain. If the domain is not set or
  // not found, it returns the default provider status.
  virtual ProviderStatus GetProviderStatus(
      std::string_view domain = "") const = 0;

  // Adds one or more global hooks. Previously added hooks are not removed.
  virtual void AddHooks(std::vector<std::shared_ptr<BaseHook>> hooks) = 0;

  // Adds a single hook to the global hook repository.
  virtual void AddHook(std::shared_ptr<BaseHook> hook) = 0;

  // Retrieves all configured global hooks.
  virtual std::vector<std::shared_ptr<BaseHook>> GetHooks() const = 0;

  // Shuts down all providers and resets the API to its initial state.
  virtual void Shutdown() = 0;
};

}  // namespace openfeature

#endif  // CPP_SDK_INCLUDE_OPENFEATURE_OPENFEATURE_H_
