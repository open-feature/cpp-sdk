#ifndef CPP_SDK_INCLUDE_OPENFEATURE_HOOKS_HOOK_ERROR_H_
#define CPP_SDK_INCLUDE_OPENFEATURE_HOOKS_HOOK_ERROR_H_

#include <string>

#include "openfeature/error_code.h"

namespace openfeature {

struct HookError {
  ErrorCode error_code{ErrorCode::kGeneral};
  std::string error_message;
};

}  // namespace openfeature

#endif  // CPP_SDK_INCLUDE_OPENFEATURE_HOOKS_HOOK_ERROR_H_
