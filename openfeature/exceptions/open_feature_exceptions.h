#ifndef CPP_SDK_INCLUDE_OPENFEATURE_OPENFEATURE_EXCEPTION_H_
#define CPP_SDK_INCLUDE_OPENFEATURE_OPENFEATURE_EXCEPTION_H_

#include <stdexcept>
#include <string>
#include <utility>

#include "openfeature/error_code.h"

namespace openfeature {

class OpenFeatureException : public std::runtime_error {
 public:
  explicit OpenFeatureException(ErrorCode error_code, std::string message = "")
      : std::runtime_error(message.empty() ? std::string(ToString(error_code))
                                           : std::move(message)),
        error_code_(error_code) {}

  ErrorCode GetErrorCode() const noexcept { return error_code_; }

 private:
  ErrorCode error_code_{ErrorCode::kGeneral};
};

}  // namespace openfeature

#endif  // CPP_SDK_INCLUDE_OPENFEATURE_OPENFEATURE_EXCEPTION_H_