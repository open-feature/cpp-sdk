#ifndef CPP_SDK_INCLUDE_OPENFEATURE_HOOK_H_
#define CPP_SDK_INCLUDE_OPENFEATURE_HOOK_H_

#include <cstdint>
#include <exception>
#include <optional>
#include <string>

#include "openfeature/base_hook.h"
#include "openfeature/evaluation_context.h"
#include "openfeature/flag_evaluation_details.h"
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
class Hook : public BaseHook {
 public:
  Hook() = default;
  Hook(const Hook&) = delete;
  Hook& operator=(const Hook&) = delete;
  Hook(Hook&&) = delete;
  Hook& operator=(Hook&&) = delete;
  ~Hook() override = default;

  // Runs before the flag evaluation occurs.
  virtual std::optional<EvaluationContext> Before(HookContext<T>& ctx,
                                                  const HookHints& hints) {
    return std::nullopt;
  }

  // Runs immediately after successful flag evaluation occurs.
  virtual void After(const HookContext<T>& ctx,
                     const FlagEvaluationDetails<T>& details,
                     const HookHints& hints) {}

  // Runs if an error occurs during flag evaluation or in `Before`/`After`
  // stages.
  virtual void Error(const HookContext<T>& ctx, const std::exception& error,
                     const HookHints& hints) {}

  // Runs after the flag evaluation occurs, regardless of whether it was
  // successful or not.
  virtual void Finally(const HookContext<T>& ctx,
                       const FlagEvaluationDetails<T>& details,
                       const HookHints& hints) {}
};

// Type aliases for common hook specializations.
using BoolHook = Hook<bool>;
using StringHook = Hook<std::string>;
using IntHook = Hook<int64_t>;
using DoubleHook = Hook<double>;
using ObjectHook = Hook<Value>;

}  // namespace openfeature

#endif  // CPP_SDK_INCLUDE_OPENFEATURE_HOOK_H_
