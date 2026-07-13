#ifndef CPP_SDK_INCLUDE_OPENFEATURE_HOOK_CONTEXT_H_
#define CPP_SDK_INCLUDE_OPENFEATURE_HOOK_CONTEXT_H_

#include <memory>
#include <string>

#include "openfeature/evaluation_context.h"
#include "openfeature/flag_metadata.h"
#include "openfeature/flag_type_value.h"
#include "openfeature/hook_data.h"
#include "openfeature/metadata.h"
#include "openfeature/value.h"

namespace openfeature {

template <typename T>
class HookContext {
 public:
  HookContext(std::string flag_key, FlagValueType type, T default_value,
              EvaluationContext ctx, Metadata client_metadata,
              Metadata provider_metadata, std::shared_ptr<HookData> hook_data);

  const std::string& GetFlagKey() const;
  FlagValueType GetType() const;
  const T& GetDefaultValue() const;
  const EvaluationContext& GetEvaluationContext() const;
  void SetEvaluationContext(EvaluationContext ctx);
  const Metadata& GetClientMetadata() const;
  const Metadata& GetProviderMetadata() const;
  std::shared_ptr<HookData> GetHookData() const;

 private:
  std::string flag_key_;
  FlagValueType type_;
  T default_value_;
  EvaluationContext ctx_;
  Metadata client_metadata_;
  Metadata provider_metadata_;
  std::shared_ptr<HookData> hook_data_;
};

// Type aliases for common types.
using BoolHookContext = HookContext<bool>;
using StringHookContext = HookContext<std::string>;
using IntHookContext = HookContext<int64_t>;
using DoubleHookContext = HookContext<double>;
using ObjectHookContext = HookContext<Value>;

}  // namespace openfeature
#endif  // CPP_SDK_INCLUDE_OPENFEATURE_HOOK_CONTEXT_H_
