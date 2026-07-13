#include "openfeature/hook_context.h"

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
HookContext<T>::HookContext(std::string flag_key, FlagValueType type,
                            T default_value, EvaluationContext ctx,
                            Metadata client_metadata, Metadata provider_metadata,
                            std::shared_ptr<HookData> hook_data)
    : flag_key_(std::move(flag_key)),
      type_(type),
      default_value_(std::move(default_value)),
      ctx_(std::move(ctx)),
      client_metadata_(std::move(client_metadata)),
      provider_metadata_(std::move(provider_metadata)),
      hook_data_(std::move(hook_data)) {}

template <typename T>
const std::string& HookContext<T>::GetFlagKey() const { return flag_key_; }

template <typename T>
FlagValueType HookContext<T>::GetType() const { return type_; }

template <typename T>
const T& HookContext<T>::GetDefaultValue() const { return default_value_; }

template <typename T>
const EvaluationContext& HookContext<T>::GetEvaluationContext() const {
  return ctx_;
}

template <typename T>
void HookContext<T>::SetEvaluationContext(EvaluationContext ctx) {
  ctx_ = std::move(ctx);
}

template <typename T>
const Metadata& HookContext<T>::GetClientMetadata() const {
  return client_metadata_;
}

template <typename T>
const Metadata& HookContext<T>::GetProviderMetadata() const {
  return provider_metadata_;
}

template <typename T>
std::shared_ptr<HookData> HookContext<T>::GetHookData() const {
  return hook_data_;
}

template class HookContext<bool>;
template class HookContext<std::string>;
template class HookContext<int64_t>;
template class HookContext<double>;
template class HookContext<Value>;

}  // namespace openfeature
