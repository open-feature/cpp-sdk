#include "openfeature/memory_provider/in_memory_provider.h"

#include <mutex>
#include <optional>
#include <utility>

#include "absl/status/statusor.h"
#include "openfeature/error_code.h"
#include "openfeature/memory_provider/flag.h"
#include "openfeature/reason.h"

namespace openfeature {

static constexpr std::string_view kName = "InMemoryProvider";

InMemoryProvider::InMemoryProvider(
    std::unordered_map<std::string, std::any> flags)
    : flags_(std::move(flags)), status_(ProviderStatus::kNotReady) {}

Metadata InMemoryProvider::GetMetadata() const {
  return Metadata{std::string(kName)};
}

absl::Status InMemoryProvider::Init(const EvaluationContext& ctx) {
  {
    std::unique_lock lock(mutex_);
    status_ = ProviderStatus::kReady;
  }
  return absl::OkStatus();
}

absl::Status InMemoryProvider::Shutdown() {
  {
    std::unique_lock lock(mutex_);
    status_ = ProviderStatus::kNotReady;
  }
  return absl::OkStatus();
}

void InMemoryProvider::UpdateFlags(
    std::unordered_map<std::string, std::any> new_flags) {
  std::unique_lock lock(mutex_);
  for (auto& [key, value] : new_flags) {
    flags_[key] = value;
  }
}

void InMemoryProvider::UpdateFlag(std::string key, std::any new_flag) {
  std::unique_lock lock(mutex_);
  flags_.insert_or_assign(std::move(key), std::move(new_flag));
}

std::unique_ptr<BoolResolutionDetails> InMemoryProvider::GetBooleanEvaluation(
    std::string_view key, bool default_value, const EvaluationContext& ctx) {
  return Evaluate<bool>(key, default_value, ctx);
}

template <typename T>
std::unique_ptr<ResolutionDetails<T>> InMemoryProvider::Evaluate(
    std::string_view key, T default_value, const EvaluationContext& ctx) {
  std::shared_lock lock(mutex_);

  if (status_ != ProviderStatus::kReady) {
    if (status_ == ProviderStatus::kNotReady) {
      return std::make_unique<ResolutionDetails<T>>(
          default_value, Reason::kError, std::nullopt, FlagMetadata{},
          ErrorCode::kProviderNotReady, "Provider is not ready");
    }
    if (status_ == ProviderStatus::kFatal) {
      return std::make_unique<ResolutionDetails<T>>(
          default_value, Reason::kError, std::nullopt, FlagMetadata{},
          ErrorCode::kProviderFatal, "Provider is in fatal error state");
    }
    return std::make_unique<ResolutionDetails<T>>(
        default_value, Reason::kError, std::nullopt, FlagMetadata{},
        ErrorCode::kGeneral, "Unknown error");
  }

  std::string key_str{key};
  auto it = flags_.find(key_str);
  if (it == flags_.end()) {
    return std::make_unique<ResolutionDetails<T>>(
        default_value, Reason::kError, std::nullopt, FlagMetadata{},
        ErrorCode::kFlagNotFound, "Flag " + key_str + " not found");
  }

  const Flag<T>* flag = std::any_cast<Flag<T>>(&it->second);

  if (!flag) {
    return std::make_unique<ResolutionDetails<T>>(
        default_value, Reason::kError, std::nullopt, FlagMetadata{},
        ErrorCode::kTypeMismatch, "Flag type mismatch");
  }

  if (flag->IsDisabled()) {
    return std::make_unique<ResolutionDetails<T>>(
        default_value, Reason::kDisabled, std::nullopt,
        flag->GetFlagMetadata());
  }

  T value;
  Reason reason = Reason::kStatic;
  const std::optional<std::string>& variant_key = flag->GetDefaultVariant();
  bool context_eval_success = false;
  const auto& evaluator = flag->GetContextEvaluator();

  if (evaluator != nullptr) {
    absl::StatusOr<T> result = evaluator(*flag, ctx);

    if (result.ok()) {
      value = *result;
      reason = Reason::kTargetingMatch;
      context_eval_success = true;
    } else {
      reason = Reason::kDefault;
    }
  }

  // Fallback to default variant if context evaluation failed.
  if (!context_eval_success) {
    if (variant_key.has_value()) {
      const std::unordered_map<std::string, T>& variants = flag->GetVariants();
      auto variant_it = variants.find(*variant_key);

      if (variant_it != variants.end()) {
        value = variant_it->second;
      } else {
        return std::make_unique<ResolutionDetails<T>>(
            default_value, Reason::kError, variant_key, flag->GetFlagMetadata(),
            ErrorCode::kParseError,
            "Default variant " + *variant_key + " not found in variants map");
      }
    } else {
      return std::make_unique<ResolutionDetails<T>>(
          default_value, Reason::kDefault, std::nullopt,
          flag->GetFlagMetadata());
    }
  }

  return std::make_unique<ResolutionDetails<T>>(value, reason, variant_key,
                                                flag->GetFlagMetadata());
}

}  // namespace openfeature
