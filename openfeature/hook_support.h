#ifndef OPENFEATURE_HOOK_SUPPORT_H_
#define OPENFEATURE_HOOK_SUPPORT_H_

#include <exception>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

#include "openfeature/error_code.h"
#include "openfeature/evaluation_context.h"
#include "openfeature/evaluation_options.h"
#include "openfeature/flag_evaluation_details.h"
#include "openfeature/flag_type_value.h"
#include "openfeature/general_hook.h"
#include "openfeature/global_hook_manager.h"
#include "openfeature/hook_context.h"
#include "openfeature/hook_data.h"
#include "openfeature/hook_hints.h"
#include "openfeature/metadata.h"
#include "openfeature/provider.h"
#include "openfeature/reason.h"

namespace openfeature {

// Helper class responsible for hook collection
// and lifecycle stage execution (before, after, error, finally).
class HookSupport {
 public:
  // Collects hooks across API, Client, Invocation, and Provider tiers in FIFO
  // order.
  static std::vector<std::shared_ptr<GeneralHook>> CollectHooks(
      const std::vector<std::shared_ptr<GeneralHook>>& client_hooks,
      const std::optional<EvaluationOptions>& options,
      const std::shared_ptr<FeatureProvider>& provider);

  // Allocates an HookData instance for each unique hook instance.
  static std::unordered_map<const GeneralHook*, std::shared_ptr<HookData>>
  CreateHookDataMap(const std::vector<std::shared_ptr<GeneralHook>>& hooks);

  // Executes before hooks in forward order (API -> Client -> Invocation ->
  // Provider).
  template <typename ValueType>
  static bool ExecuteBeforeHooks(
      const std::vector<std::shared_ptr<GeneralHook>>& hooks,
      std::string_view flag_key, FlagValueType flag_type,
      const ValueType& default_value, const Metadata& client_metadata,
      const Metadata& provider_metadata, const HookHints& hints,
      std::unordered_map<const GeneralHook*, std::shared_ptr<HookData>>&
          hook_data_map,
      EvaluationContext& merged_context, std::optional<ErrorCode>& error_code,
      std::string& error_message,
      std::unique_ptr<std::exception>& captured_exception) {
    for (const auto& hook : hooks) {
      try {
        HookContext<ValueType> hook_ctx(
            std::string(flag_key), flag_type, default_value, merged_context,
            client_metadata, provider_metadata, hook_data_map[hook.get()]);
        auto modified_ctx = hook->Before(hook_ctx, hints);
        if (modified_ctx.has_value()) {
          merged_context =
              EvaluationContext::Merge({&merged_context, &(*modified_ctx)});
        }
      } catch (const std::exception& exception) {
        error_code = ErrorCode::kGeneral;
        error_message = exception.what();
        captured_exception =
            std::make_unique<std::runtime_error>(exception.what());
        return false;
      } catch (...) {
        error_code = ErrorCode::kGeneral;
        error_message = "Unknown exception in before hook";
        captured_exception =
            std::make_unique<std::runtime_error>(error_message);
        return false;
      }
    }
    return true;
  }

  // Executes after hooks in reverse order (Provider -> Invocation -> Client ->
  // API).
  template <typename ValueType>
  static void ExecuteAfterHooks(
      const std::vector<std::shared_ptr<GeneralHook>>& reverse_hooks,
      std::string_view flag_key, FlagValueType flag_type,
      const ValueType& default_value, const EvaluationContext& merged_context,
      const Metadata& client_metadata, const Metadata& provider_metadata,
      const HookHints& hints,
      std::unordered_map<const GeneralHook*, std::shared_ptr<HookData>>&
          hook_data_map,
      std::unique_ptr<FlagEvaluationDetails<ValueType>>& evaluation_details,
      std::optional<ErrorCode>& error_code, std::string& error_message,
      std::unique_ptr<std::exception>& captured_exception, bool& has_error) {
    for (const auto& hook : reverse_hooks) {
      try {
        HookContext<ValueType> hook_ctx(
            std::string(flag_key), flag_type, default_value, merged_context,
            client_metadata, provider_metadata, hook_data_map[hook.get()]);
        hook->After(hook_ctx, *evaluation_details, hints);
      } catch (const std::exception& exception) {
        has_error = true;
        error_code = ErrorCode::kGeneral;
        error_message = exception.what();
        captured_exception =
            std::make_unique<std::runtime_error>(exception.what());
        evaluation_details = std::make_unique<FlagEvaluationDetails<ValueType>>(
            std::string(flag_key), default_value, Reason::kError, std::nullopt,
            FlagMetadata(), error_code.value_or(ErrorCode::kGeneral),
            error_message);
        break;
      } catch (...) {
        has_error = true;
        error_code = ErrorCode::kGeneral;
        error_message = "Unknown exception in after hook";
        captured_exception =
            std::make_unique<std::runtime_error>(error_message);
        evaluation_details = std::make_unique<FlagEvaluationDetails<ValueType>>(
            std::string(flag_key), default_value, Reason::kError, std::nullopt,
            FlagMetadata(), error_code.value_or(ErrorCode::kGeneral),
            error_message);
        break;
      }
    }
  }

  // Executes error hooks in reverse order (Provider -> Invocation -> Client ->
  // API).
  template <typename ValueType>
  static void ExecuteErrorHooks(
      const std::vector<std::shared_ptr<GeneralHook>>& reverse_hooks,
      std::string_view flag_key, FlagValueType flag_type,
      const ValueType& default_value, const EvaluationContext& merged_context,
      const Metadata& client_metadata, const Metadata& provider_metadata,
      const HookHints& hints,
      std::unordered_map<const GeneralHook*, std::shared_ptr<HookData>>&
          hook_data_map,
      const std::exception& captured_exception) {
    for (const auto& hook : reverse_hooks) {
      try {
        HookContext<ValueType> hook_ctx(
            std::string(flag_key), flag_type, default_value, merged_context,
            client_metadata, provider_metadata, hook_data_map[hook.get()]);
        hook->Error(hook_ctx, captured_exception, hints);
      } catch (...) {
        // evaluation must proceed
      }
    }
  }

  // Executes finally hooks in reverse order (Provider -> Invocation -> Client
  // -> API).
  template <typename ValueType>
  static void ExecuteFinallyHooks(
      const std::vector<std::shared_ptr<GeneralHook>>& reverse_hooks,
      std::string_view flag_key, FlagValueType flag_type,
      const ValueType& default_value, const EvaluationContext& merged_context,
      const Metadata& client_metadata, const Metadata& provider_metadata,
      const HookHints& hints,
      std::unordered_map<const GeneralHook*, std::shared_ptr<HookData>>&
          hook_data_map,
      const FlagEvaluationDetails<ValueType>& evaluation_details) {
    for (const auto& hook : reverse_hooks) {
      try {
        HookContext<ValueType> hook_ctx(
            std::string(flag_key), flag_type, default_value, merged_context,
            client_metadata, provider_metadata, hook_data_map[hook.get()]);
        hook->Finally(hook_ctx, evaluation_details, hints);
      } catch (...) {
        // evaluation must proceed
      }
    }
  }
};

}  // namespace openfeature

#endif  // OPENFEATURE_HOOK_SUPPORT_H_
