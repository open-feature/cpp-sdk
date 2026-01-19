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

namespace openfeature {

// OpenFeature client implementation.
class ClientAPI : public Client, public Features {
 public:
  // ClientAPI(std::shared_ptr<OpenFeatureAPI> api, std::string_view domain);
  ClientAPI(std::string_view domain);

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

  bool GetBooleanValue(std::string_view flag_key, bool default_value) override;
  bool GetBooleanValue(std::string_view flag_key, bool default_value,
                       const EvaluationContext& ctx) override;

  // TODO: Add methods to get and set Hooks
  // TODO: Add methods for flag evaluation for other types (e.g. string, int,
  // float, object)
  // TODO: Add methods for detailed flag evaluation
  // TODO: Overload method "GetBooleanValue" to accept "Evaluation Options"

 private:
  std::unique_ptr<BoolResolutionDetails> EvaluateBooleanFlag(
      std::string_view flag_key, bool default_value,
      const std::optional<EvaluationContext>& ctx);

  EvaluationContext MergeContexts(
      const std::optional<EvaluationContext>& invocation_ctx);

  ProviderRepository provider_repository_;
  std::string domain_;
  EvaluationContext evaluation_context_;
  mutable std::mutex context_mutex_;
};

}  // namespace openfeature

#endif  // CPP_SDK_INCLUDE_OPENFEATURE_CLIENT_API_H_
