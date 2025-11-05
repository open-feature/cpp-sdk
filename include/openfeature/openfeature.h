#ifndef CPP_SDK_INCLUDE_OPENFEATURE_OPENFEATURE_H_
#define CPP_SDK_INCLUDE_OPENFEATURE_OPENFEATURE_H_

#include "openfeature/client.h"
#include "openfeature/evaluation_context.h"
#include "openfeature/provider.h"

#include <chrono>
#include <string>
#include <string_view>
#include <memory>

namespace openfeature {

class OpenFeature{
    public:
        virtual ~OpenFeature() = default;

        // Sets the default provider.
        virtual void SetProvider(std::shared_ptr<FeatureProvider> provider) = 0;

        // Sets a provider for a specific domain.
        virtual void SetProvider(std::string_view domain, std::shared_ptr<FeatureProvider> provider) = 0;

        // Sets the default provider and blocks until it successfully initializes.
        virtual void SetProviderAndWait(std::shared_ptr<FeatureProvider> provider) = 0;

        // Sets the default provider and blocks until it initializes or a timeout occurs.
        virtual void SetProviderAndWait(std::shared_ptr<FeatureProvider> provider, std::chrono::milliseconds timeout) = 0;

        // Sets a named provider and blocks until it successfully initializes.
        virtual void SetProviderAndWait(std::string_view domain, std::shared_ptr<FeatureProvider> provider) = 0;
        
        // Sets a named provider and blocks until it initializes or a timeout occurs.
        virtual void SetProviderAndWait(std::string_view domain,  std::shared_ptr<FeatureProvider> provider, std::chrono::milliseconds timeout) = 0;

        virtual std::shared_ptr<Client> GetClient() = 0;

        virtual std::shared_ptr<Client> GetClient(std::string_view domain) = 0;

        // Sets the global evaluation context.
        virtual void SetEvaluationContext(const EvaluationContext& ctx) = 0;

        // Gets the metadata for the default provider.
        virtual Metadata GetProviderMetadata() = 0;

        // Gets the metadata for a provider bound to a specific domain.
        virtual Metadata GetProviderMetadata(std::string_view domain) = 0;

        // Shuts down all providers and resets the API to its initial state.
        virtual void Shutdown() = 0;
};

} // namespace openfeature

#endif CPP_SDK_INCLUDE_OPENFEATURE_OPENFEATURE_H_
