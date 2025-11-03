#ifndef CPP_SDK_INCLUDE_OPENFEATURE_CLIENT_H_
#define CPP_SDK_INCLUDE_OPENFEATURE_CLIENT_H_

#include "openfeature/evaluation_context.h"
#include "openfeature/features.h"
#include "openfeature/metadata.h"
#include "openfeature/provider_status.h"

namespace openfeature {

// OpenFeature client implementation.
class Client: public Features {
    public:
        virtual ~Client() = default;
        virtual Metadata GetMetadata() = 0;

        // Return an optional client-level evaluation context.
        virtual EvaluationContext GetEvaluationContext() = 0;

        // Set the client-level evaluation context.
        virtual void SetEvaluationContext(EvaluationContext& ctx) = 0;

        // Returns the current status of the associated provider.
        virtual ProviderStatus GetProviderStatus() = 0;

        // TODO: Add methods to add and get Hooks
};
} // namespace openfeature

#endif CPP_SDK_INCLUDE_OPENFEATURE_CLIENT_H_
