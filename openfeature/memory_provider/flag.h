#ifndef CPP_SDK_INCLUDE_OPENFEATURE_FLAG_H_
#define CPP_SDK_INCLUDE_OPENFEATURE_FLAG_H_

#include <functional>
#include <string>
#include <unordered_map>
#include <utility>

#include "absl/status/statusor.h"
#include "openfeature/evaluation_context.h"
#include "openfeature/flag_metadata.h"

namespace openfeature {

// This class represents the flag structure used for the InMemoryProvider.
template <typename T>
class Flag {
 public:
  using ContextEvaluator =
      std::function<absl::StatusOr<T>(const Flag&, const EvaluationContext&)>;

  Flag(std::unordered_map<std::string, T> variants, std::string default_variant,
       ContextEvaluator context_evaluator, FlagMetadata flag_metadata,
       bool disabled = false)
      : variants_(std::move(variants)),
        default_variant_(std::move(default_variant)),
        context_evaluator_(std::move(context_evaluator)),
        flag_metadata_(std::move(flag_metadata)),
        disabled_(disabled) {}

  const std::unordered_map<std::string, T>& GetVariants() const {
    return variants_;
  }

  const std::string& GetDefaultVariant() const { return default_variant_; }

  const ContextEvaluator& GetContextEvaluator() const {
    return context_evaluator_;
  }

  const FlagMetadata& GetFlagMetadata() const { return flag_metadata_; }

  bool IsDisabled() const { return disabled_; }

 private:
  std::unordered_map<std::string, T> variants_;
  std::string default_variant_;
  ContextEvaluator context_evaluator_;
  FlagMetadata flag_metadata_;
  bool disabled_;
};
}  // namespace openfeature

#endif  // CPP_SDK_INCLUDE_OPENFEATURE_FLAG_H_
