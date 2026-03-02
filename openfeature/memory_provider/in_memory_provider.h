#ifndef CPP_SDK_INCLUDE_OPENFEATURE_IN_MEMORY_PROVIDER_H_
#define CPP_SDK_INCLUDE_OPENFEATURE_IN_MEMORY_PROVIDER_H_

#include <any>
#include <memory>
#include <shared_mutex>
#include <string>
#include <string_view>
#include <unordered_map>

#include "absl/status/status.h"
#include "openfeature/evaluation_context.h"
#include "openfeature/metadata.h"
#include "openfeature/provider.h"
#include "openfeature/provider_status.h"
#include "openfeature/resolution_details.h"
#include "openfeature/value.h"

namespace openfeature {

// This class implements the FeatureProvider interface and is intended to be
// used for testing. It stores feature flags in memory and allows for
// evaluation based on the provided EvaluationContext.
class InMemoryProvider : public FeatureProvider {
 public:
  InMemoryProvider(std::unordered_map<std::string, std::any> flags);

  ~InMemoryProvider() = default;

  Metadata GetMetadata() const override;

  absl::Status Init(const EvaluationContext& ctx) override;
  absl::Status Shutdown() override;

  // Updates the provider flags configuration. All existing flags will be
  // replaced with the new ones. If there are any new flags, they will be
  // added to the configuration.
  void UpdateFlags(std::unordered_map<std::string, std::any> new_flags);

  // Updates a single flag in the provider configuration. If the flag already
  // exists, it will be replaced with the new one. If it doesn't exist, it
  // will be added to the configuration.
  void UpdateFlag(std::string key, std::any new_flag);

  std::unique_ptr<BoolResolutionDetails> GetBooleanEvaluation(
      std::string_view key, bool default_value,
      const EvaluationContext& ctx) override;

  std::unique_ptr<StringResolutionDetails> GetStringEvaluation(
      std::string_view key, std::string_view default_value,
      const EvaluationContext& ctx) override;

  std::unique_ptr<IntResolutionDetails> GetIntegerEvaluation(
      std::string_view key, int64_t default_value,
      const EvaluationContext& ctx) override;

  std::unique_ptr<DoubleResolutionDetails> GetDoubleEvaluation(
      std::string_view key, double default_value,
      const EvaluationContext& ctx) override;

  std::unique_ptr<ObjectResolutionDetails> GetObjectEvaluation(
      std::string_view key, Value default_value,
      const EvaluationContext& ctx) override;

 private:
  template <typename T>
  std::unique_ptr<ResolutionDetails<T>> Evaluate(std::string_view key,
                                                 T default_value,
                                                 const EvaluationContext& ctx);

  std::unordered_map<std::string, std::any> flags_;
  ProviderStatus status_;
  mutable std::shared_mutex mutex_;
};

}  // namespace openfeature

#endif  // CPP_SDK_INCLUDE_OPENFEATURE_IN_MEMORY_PROVIDER_H_
