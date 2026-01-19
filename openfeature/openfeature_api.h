#ifndef CPP_SDK_INCLUDE_OPENFEATURE_OPENFEATURE_API_H_
#define CPP_SDK_INCLUDE_OPENFEATURE_OPENFEATURE_API_H_

#include <memory>
#include <shared_mutex>
#include <string_view>

#include "openfeature/client.h"
#include "openfeature/evaluation_context.h"
#include "openfeature/global_context_manager.h"
#include "openfeature/metadata.h"
#include "openfeature/openfeature.h"
#include "openfeature/provider.h"
#include "openfeature/provider_repository.h"

namespace openfeature {

// A global singleton which holds base configuration for the OpenFeature
// library.
class OpenFeatureAPI : public OpenFeature {
 public:
  ~OpenFeatureAPI();

  // Get the singleton instance of the OpenFeatureAPI.
  static OpenFeatureAPI& GetInstance();

  OpenFeatureAPI(const OpenFeatureAPI&) = delete;
  OpenFeatureAPI& operator=(const OpenFeatureAPI&) = delete;

  // Set the default provider.
  void SetProvider(std::shared_ptr<FeatureProvider> provider) override;

  // Set a provider for a specific domain.
  void SetProvider(std::string_view domain,
                   std::shared_ptr<FeatureProvider> provider) override;

  // Set the default provider and blocks until it successfully initializes.
  void SetProviderAndWait(std::shared_ptr<FeatureProvider> provider) override;

  // Set a named provider and blocks until it successfully initializes.
  void SetProviderAndWait(std::string_view domain,
                          std::shared_ptr<FeatureProvider> provider) override;
  // If the domain is empty then GetProvider returns the default provider
  // otherwise it returns the provider for the domain. If this domain has no
  // provider bound, it returns the default provider.
  std::shared_ptr<FeatureProvider> GetProvider(
      std::string_view domain = "") const override;

  // Gets a client for the default domain.
  std::shared_ptr<Client> GetClient() override;

  // Gets a client for a named domain.
  std::shared_ptr<Client> GetClient(std::string_view domain) override;

  // Sets the global evaluation context.
  void SetEvaluationContext(const EvaluationContext& ctx) override;

  // Gets the global evaluation context.
  EvaluationContext GetEvaluationContext() const override;

  // Get metadata about the default provider if domain is empty
  // or about a named provider if domain is provided.
  Metadata GetProviderMetadata(std::string_view domain = "") const override;

  // Fetches the status of a provider for a domain. If the domain is not set or
  // not found, it returns the default provider status.
  ProviderStatus GetProviderStatus(std::string_view domain = "") const override;

  // Shuts down all providers and resets the API to its initial state.
  void Shutdown() override;

  // TODO: Add methods to add and get Hooks.
  // TODO: Add overload function for "GetClient()" to accept "Evaluation
  // Options"

 private:
  ProviderRepository provider_repository_;

  OpenFeatureAPI();
};

}  // namespace openfeature

#endif  // CPP_SDK_INCLUDE_OPENFEATURE_OPENFEATURE_API_H_
