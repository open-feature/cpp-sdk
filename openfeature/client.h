#ifndef CPP_SDK_INCLUDE_OPENFEATURE_CLIENT_H_
#define CPP_SDK_INCLUDE_OPENFEATURE_CLIENT_H_

#include <memory>
#include <vector>

#include "openfeature/evaluation_context.h"
#include "openfeature/features.h"
#include "openfeature/general_hook.h"
#include "openfeature/metadata.h"
#include "openfeature/provider_status.h"

namespace openfeature {

// OpenFeature client implementation.
class Client : public Features {
 public:
  ~Client() override = default;
  virtual Metadata GetMetadata() = 0;

  // Return an optional client-level evaluation context.
  virtual EvaluationContext GetEvaluationContext() = 0;

  // Set the client-level evaluation context.
  virtual void SetEvaluationContext(const EvaluationContext& ctx) = 0;

  // Returns the current status of the associated provider.
  virtual ProviderStatus GetProviderStatus() = 0;

  // Adds one or more hooks to the client-level hook repository.
  virtual void AddHooks(std::vector<std::shared_ptr<GeneralHook>> hooks) = 0;

  // Adds a single hook to the client-level hook repository.
  virtual void AddHook(std::shared_ptr<GeneralHook> hook) = 0;

  // Retrieves all configured client-level hooks.
  virtual std::vector<std::shared_ptr<GeneralHook>> GetHooks() const = 0;
};

}  // namespace openfeature

#endif  // CPP_SDK_INCLUDE_OPENFEATURE_CLIENT_H_
