#ifndef CPP_SDK_INCLUDE_OPENFEATURE_GENERAL_HOOK_H_
#define CPP_SDK_INCLUDE_OPENFEATURE_GENERAL_HOOK_H_

#include <cstdint>
#include <exception>
#include <optional>

#include "openfeature/evaluation_context.h"
#include "openfeature/general_flag_evaluation_details.h"
#include "openfeature/general_hook_context.h"
#include "openfeature/hook_hints.h"
#include "openfeature/value.h"

namespace openfeature {

// Non-templated general class for all hooks. This will allow storing different
// hook types (e.g., BoolHook,
// StringHook) inside evaluation options.
class GeneralHook {
 public:
  GeneralHook() = default;
  GeneralHook(const GeneralHook&) = delete;
  GeneralHook(GeneralHook&&) = delete;
  GeneralHook& operator=(const GeneralHook&) = delete;
  GeneralHook& operator=(GeneralHook&&) = delete;
  virtual ~GeneralHook() = default;

  // 1. Before: Runs before flag evaluation occurs.
  virtual std::optional<EvaluationContext> Before(GeneralHookContext& ctx,
                                                  const HookHints& hints) {
    return std::nullopt;
  }
  // 2. After: Runs immediately after successful flag evaluation.
  virtual void After(const GeneralHookContext& ctx,
                     const GeneralFlagEvaluationDetails& details,
                     const HookHints& hints) {}
  // 3. Error: Runs if an error occurs during evaluation or in Before/After.
  virtual void Error(const GeneralHookContext& ctx, const std::exception& error,
                     const HookHints& hints) {}
  // 4. Finally: Runs after evaluation occurs, regardless of success or error.
  virtual void Finally(const GeneralHookContext& ctx,
                       const GeneralFlagEvaluationDetails& details,
                       const HookHints& hints) {}
};

}  // namespace openfeature

#endif  // CPP_SDK_INCLUDE_OPENFEATURE_GENERAL_HOOK_H_