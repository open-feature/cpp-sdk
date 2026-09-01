#include "openfeature/hooks/logging_hook.h"

#include <any>
#include <exception>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <utility>

#include "openfeature/evaluation_context.h"
#include "openfeature/exceptions/open_feature_exceptions.h"
#include "openfeature/general_flag_evaluation_details.h"
#include "openfeature/general_hook.h"
#include "openfeature/general_hook_context.h"
#include "openfeature/hook_hints.h"
#include "openfeature/reason.h"
#include "openfeature/value.h"

namespace openfeature {

LoggingHook::LoggingHook(bool include_eval_context, LogCallback logger)
    : include_eval_context_(include_eval_context), logger_(std::move(logger)) {
  if (!logger_) {
    // Default logger prints to stderr/stdout
    logger_ = [](LogLevel level, const std::string& msg) {
      if (level == LogLevel::kError) {
        std::cerr << "[ERROR] " << msg << std::endl;
      } else {
        std::cout << "[DEBUG] " << msg << std::endl;
      }
    };
  }
}

void LoggingHook::Log(LogLevel level, const std::string& msg) const {
  if (logger_) {
    logger_(level, msg);
  }
}

std::optional<EvaluationContext> LoggingHook::Before(
    const GeneralHookContext& ctx, const HookHints& hints) {
  std::ostringstream ss;
  ss << "stage=before"
     << ", domain=\"" << ctx.GetClientMetadata().name << "\""
     << ", provider_name=\"" << ctx.GetProviderMetadata().name << "\""
     << ", flag_key=\"" << ctx.GetFlagKey() << "\""
     << ", default_value=" << ctx.GetDefaultValueAsValue().ToString();

  if (include_eval_context_) {
    ss << ", evaluation_context=" << ctx.GetEvaluationContext().ToString();
  }

  Log(LogLevel::kDebug, ss.str());
  return std::nullopt;
}

void LoggingHook::After(const GeneralHookContext& ctx,
                        const GeneralFlagEvaluationDetails& details,
                        const HookHints& hints) {
  std::ostringstream ss;
  ss << "stage=after"
     << ", domain=\"" << ctx.GetClientMetadata().name << "\""
     << ", provider_name=\"" << ctx.GetProviderMetadata().name << "\""
     << ", flag_key=\"" << ctx.GetFlagKey() << "\""
     << ", default_value=" << ctx.GetDefaultValueAsValue().ToString()
     << ", reason=\"" << ToString(details.GetReason()) << "\""
     << ", variant="
     << (details.GetVariant() ? ("\"" + *details.GetVariant() + "\"") : "null")
     << ", value=" << details.GetValueAsValue().ToString();
  if (include_eval_context_) {
    ss << ", evaluation_context=" << ctx.GetEvaluationContext().ToString();
  }
  Log(LogLevel::kDebug, ss.str());
}

void LoggingHook::Error(const GeneralHookContext& ctx,
                        const std::exception& error, const HookHints& hints) {
  ErrorCode code = ErrorCode::kGeneral;
  if (const auto* of_err = dynamic_cast<const OpenFeatureException*>(&error)) {
    code = of_err->GetErrorCode();
  }
  std::ostringstream ss;
  ss << "stage=error"
     << ", domain=\"" << ctx.GetClientMetadata().name << "\""
     << ", provider_name=\"" << ctx.GetProviderMetadata().name << "\""
     << ", flag_key=\"" << ctx.GetFlagKey() << "\""
     << ", default_value=" << ctx.GetDefaultValueAsValue().ToString()
     << ", error_code=\"" << ToString(code) << "\""
     << ", error_message=\"" << error.what() << "\"";

  if (include_eval_context_) {
    ss << ", evaluation_context=" << ctx.GetEvaluationContext().ToString();
  }
  Log(LogLevel::kError, ss.str());
}

void LoggingHook::Finally(const GeneralHookContext& ctx,
                          const GeneralFlagEvaluationDetails& details,
                          const HookHints& hints) {
  // Stage `finally` is a no-op per Appendix A
}

}  // namespace openfeature
