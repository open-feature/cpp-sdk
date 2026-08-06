#ifndef CPP_SDK_INCLUDE_OPENFEATURE_GENERAL_HOOK_CONTEXT_H_
#define CPP_SDK_INCLUDE_OPENFEATURE_GENERAL_HOOK_CONTEXT_H_

#include <memory>
#include <string>

#include "openfeature/evaluation_context.h"
#include "openfeature/flag_type_value.h"
#include "openfeature/hook_data.h"
#include "openfeature/metadata.h"
#include "openfeature/value.h"

namespace openfeature {

class GeneralHookContext {
 public:
  virtual ~GeneralHookContext() = default;
  virtual const std::string& GetFlagKey() const = 0;
  virtual FlagValueType GetType() const = 0;
  virtual Value GetDefaultValueAsValue() const = 0;
  virtual const EvaluationContext& GetEvaluationContext() const = 0;
  virtual void SetEvaluationContext(EvaluationContext ctx) = 0;
  virtual const Metadata& GetClientMetadata() const = 0;
  virtual const Metadata& GetProviderMetadata() const = 0;
  virtual std::shared_ptr<HookData> GetHookData() const = 0;
};

}  // namespace openfeature

#endif  // CPP_SDK_INCLUDE_OPENFEATURE_GENERAL_HOOK_CONTEXT_H_