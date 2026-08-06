#ifndef CPP_SDK_INCLUDE_OPENFEATURE_HOOK_H_
#define CPP_SDK_INCLUDE_OPENFEATURE_HOOK_H_

#include <cstdint>
#include <exception>
#include <optional>
#include <string>

#include "openfeature/evaluation_context.h"
#include "openfeature/flag_evaluation_details.h"
#include "openfeature/general_flag_evaluation_details.h"
#include "openfeature/general_hook.h"
#include "openfeature/general_hook_context.h"
#include "openfeature/hook_context.h"
#include "openfeature/hook_hints.h"
#include "openfeature/value.h"

namespace openfeature {

// Hook allows application developers to add arbitrary behavior to the
// flag evaluation lifecycle. Hooks operate similarly to middleware in web
// frameworks. They are executed stack-wise with respect to flag resolution,
// prioritizing increasing specificity (API, Client, Invocation, Provider)
// first, and the order in which they were added second.
/// https://openfeature.dev/specification/sections/hooks
template <typename T>
class Hook : public GeneralHook {
 public:
  Hook() = default;
  Hook(const Hook&) = delete;
  Hook& operator=(const Hook&) = delete;
  Hook(Hook&&) = delete;
  Hook& operator=(Hook&&) = delete;
  ~Hook() override = default;

  // Runs before the flag evaluation occurs.
  std::optional<EvaluationContext> Before(const GeneralHookContext& ctx,
                                          const HookHints& hints) final {
    if (auto* typed_ctx = dynamic_cast<const HookContext<T>*>(&ctx)) {
      return Before(*typed_ctx, hints);
    }
    return std::nullopt;
  }

  // Runs immediately after successful flag evaluation occurs.
  void After(const GeneralHookContext& ctx,
             const GeneralFlagEvaluationDetails& details,
             const HookHints& hints) final {
    auto* typed_ctx = dynamic_cast<const HookContext<T>*>(&ctx);
    auto* typed_details =
        dynamic_cast<const FlagEvaluationDetails<T>*>(&details);
    if (typed_ctx && typed_details) {
      After(*typed_ctx, *typed_details, hints);
    }
  }

  // Runs if an error occurs during flag evaluation or in `Before`/`After`
  // stages.
  void Error(const GeneralHookContext& ctx, const std::exception& error,
             const HookHints& hints) final {
    if (auto* typed_ctx = dynamic_cast<const HookContext<T>*>(&ctx)) {
      Error(*typed_ctx, error, hints);
    }
  }

  // Runs after the flag evaluation occurs, regardless of whether it was
  // successful or not.
  void Finally(const GeneralHookContext& ctx,
               const GeneralFlagEvaluationDetails& details,
               const HookHints& hints) final {
    auto* typed_ctx = dynamic_cast<const HookContext<T>*>(&ctx);
    auto* typed_details =
        dynamic_cast<const FlagEvaluationDetails<T>*>(&details);
    if (typed_ctx && typed_details) {
      Finally(*typed_ctx, *typed_details, hints);
    }
  }

  // Typed virtual methods for type-specific subclasses to override:
  virtual std::optional<EvaluationContext> Before(const HookContext<T>& ctx,
                                                  const HookHints& hints) {
    return std::nullopt;
  }
  virtual void After(const HookContext<T>& ctx,
                     const FlagEvaluationDetails<T>& details,
                     const HookHints& hints) {}
  virtual void Error(const HookContext<T>& ctx, const std::exception& error,
                     const HookHints& hints) {}
  virtual void Finally(const HookContext<T>& ctx,
                       const FlagEvaluationDetails<T>& details,
                       const HookHints& hints) {}
};

using BoolHook = Hook<bool>;
using StringHook = Hook<std::string>;
using IntHook = Hook<int64_t>;
using DoubleHook = Hook<double>;
using ObjectHook = Hook<Value>;
}  // namespace openfeature

#endif  // CPP_SDK_INCLUDE_OPENFEATURE_HOOK_H_
