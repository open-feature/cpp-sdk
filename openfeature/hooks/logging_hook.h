#ifndef CPP_SDK_INCLUDE_OPENFEATURE_HOOKS_LOGGING_HOOK_H_
#define CPP_SDK_INCLUDE_OPENFEATURE_HOOKS_LOGGING_HOOK_H_

#include <exception>
#include <functional>
#include <optional>
#include <string>

#include "openfeature/evaluation_context.h"
#include "openfeature/general_flag_evaluation_details.h"
#include "openfeature/general_hook.h"
#include "openfeature/general_hook_context.h"
#include "openfeature/hook_hints.h"
#include "openfeature/hooks/log_level.h"

namespace openfeature {

using LogCallback =
    std::function<void(LogLevel level, const std::string& message)>;

// A hook for logging flag evaluations.
// Useful for debugging.
// Flag evaluation data is logged at debug and error in before/after stages and
// error stages, respectively.
class LoggingHook : public GeneralHook {
 public:
  LoggingHook(bool include_eval_context = false, LogCallback logger = nullptr);

  std::optional<EvaluationContext> Before(const GeneralHookContext& ctx,
                                          const HookHints& hints) override;
  void After(const GeneralHookContext& ctx,
             const GeneralFlagEvaluationDetails& details,
             const HookHints& hints) override;
  void Error(const GeneralHookContext& ctx, const std::exception& error,
             const HookHints& hints) override;
  void Finally(const GeneralHookContext& ctx,
               const GeneralFlagEvaluationDetails& details,
               const HookHints& hints) override;

 private:
  void Log(LogLevel level, const std::string& msg) const;

  bool include_eval_context_{false};
  LogCallback logger_;
};

}  // namespace openfeature

#endif  // CPP_SDK_INCLUDE_OPENFEATURE_HOOKS_LOGGING_HOOK_H_
