#ifndef CPP_SDK_INCLUDE_OPENFEATURE_PROVIDER_EVALUATION_H_
#define CPP_SDK_INCLUDE_OPENFEATURE_PROVIDER_EVALUATION_H_

#include <string>

#include "openfeature/error_code.h"
#include "openfeature/flag_metadata.h"
#include "openfeature/reason.h"

namespace openfeature {

// This is a common interface between the evaluation results that providers
// return and what is given to the end users. <T> The type of flag being
// evaluated. see https://openfeature.dev/specification/types#resolution-details
template <typename T>
class ProviderEvaluation {
 public:
  virtual ~ProviderEvaluation() = default;
  virtual T GetValue() const = 0;
  virtual std::string GetVariant() const = 0;
  virtual Reason GetReason() const = 0;
  virtual ErrorCode GetErrorCode() const = 0;
  virtual std::string GetErrorMessage() const = 0;
  virtual const FlagMetadata& GetFlagMetadata() const = 0;
};

}  // namespace openfeature

#endif CPP_SDK_INCLUDE_OPENFEATURE_PROVIDER_EVALUATION_H_
