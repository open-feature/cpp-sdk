#ifndef CPP_SDK_INCLUDE_OPENFEATURE_GENERAL_FLAG_EVALUATION_DETAILS_H_
#define CPP_SDK_INCLUDE_OPENFEATURE_GENERAL_FLAG_EVALUATION_DETAILS_H_

#include <optional>
#include <string>
#include <string_view>

#include "openfeature/error_code.h"
#include "openfeature/flag_metadata.h"
#include "openfeature/reason.h"
#include "openfeature/value.h"

namespace openfeature {

class GeneralFlagEvaluationDetails {
 public:
  virtual ~GeneralFlagEvaluationDetails() = default;
  virtual std::string_view GetFlagKey() const = 0;
  virtual Value GetValueAsValue() const = 0;
  virtual Reason GetReason() const = 0;
  virtual std::optional<std::string> GetVariant() const = 0;
  virtual const FlagMetadata& GetFlagMetadata() const = 0;
  virtual std::optional<ErrorCode> GetErrorCode() const = 0;
  virtual std::optional<std::string> GetErrorMessage() const = 0;
};

}  // namespace openfeature

#endif  // CPP_SDK_INCLUDE_OPENFEATURE_GENERAL_FLAG_EVALUATION_DETAILS_H_
