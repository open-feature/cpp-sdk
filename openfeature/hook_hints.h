#ifndef CPP_SDK_INCLUDE_OPENFEATURE_HOOK_HINTS_H_
#define CPP_SDK_INCLUDE_OPENFEATURE_HOOK_HINTS_H_

#include <any>
#include <string>
#include <unordered_map>

namespace openfeature {

using HookHints = std::unordered_map<std::string, std::any>;

}  // namespace openfeature

#endif  // CPP_SDK_INCLUDE_OPENFEATURE_HOOK_HINTS_H_
