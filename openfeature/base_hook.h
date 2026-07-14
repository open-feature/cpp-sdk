#ifndef CPP_SDK_INCLUDE_OPENFEATURE_BASE_HOOK_H_
#define CPP_SDK_INCLUDE_OPENFEATURE_BASE_HOOK_H_

namespace openfeature {

// Non-templated base class for all hooks. This will allow storing different
// hook types (e.g., BoolHook,
// StringHook) inside evaluation options.
class BaseHook {
 public:
  BaseHook() = default;
  BaseHook(const BaseHook&) = delete;
  BaseHook(BaseHook&&) = default;
  BaseHook& operator=(const BaseHook&) = delete;
  BaseHook& operator=(BaseHook&&) = default;
  virtual ~BaseHook() = default;
};

}  // namespace openfeature

#endif  // CPP_SDK_INCLUDE_OPENFEATURE_BASE_HOOK_H_