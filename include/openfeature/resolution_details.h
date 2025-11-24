#ifndef CPP_SDK_INCLUDE_OPENFEATURE_RESOLUTION_DETAILS_H_
#define CPP_SDK_INCLUDE_OPENFEATURE_RESOLUTION_DETAILS_H_

#include <any>
#include <optional>
#include <string>

#include "openfeature/error_code.h"
#include "openfeature/flag_metadata.h"
#include "openfeature/reason.h"

namespace openfeature {

// This is a common interface between the evaluation results that providers
// return and what is given to the end users. <T> The type of flag being
// evaluated. see https://openfeature.dev/specification/types#resolution-details
template <typename T>
class ResolutionDetails {
 public:
  ResolutionDetails(T value, Reason reason, std::optional<std::string> variant,
                    const FlagMetadata& flag_metadata,
                    std::optional<ErrorCode> error_code = std::nullopt,
                    std::optional<std::string> error_message = std::nullopt);

  ~ResolutionDetails() = default;
  T GetValue() const;
  std::optional<std::string> GetVariant() const;
  Reason GetReason() const;
  std::optional<ErrorCode> GetErrorCode() const;
  std::optional<std::string> GetErrorMessage() const;
  const FlagMetadata& GetFlagMetadata() const;

 private:
  T value_;
  std::optional<ErrorCode> error_code_;
  std::optional<std::string> error_message_;
  Reason reason_;
  std::optional<std::string> variant_;
  FlagMetadata flag_metadata_;
};

// Type aliases for common types
using BoolResolutionDetails = ResolutionDetails<bool>;
using StringResolutionDetails = ResolutionDetails<std::string>;
using IntResolutionDetails = ResolutionDetails<int64_t>;
using DoubleResolutionDetails = ResolutionDetails<double>;
using ObjectResolutionDetails = ResolutionDetails<std::any>;

}  // namespace openfeature

#endif  // CPP_SDK_INCLUDE_OPENFEATURE_RESOLUTION_DETAILS_H_
