#ifndef CPP_SDK_INCLUDE_OPENFEATURE_FLAG_EVALUATION_DETAILS_H_
#define CPP_SDK_INCLUDE_OPENFEATURE_FLAG_EVALUATION_DETAILS_H_

#include <optional>
#include <string>
#include <string_view>

#include "openfeature/error_code.h"
#include "openfeature/flag_metadata.h"
#include "openfeature/reason.h"
#include "openfeature/resolution_details.h"
#include "openfeature/value.h"

namespace openfeature {

template <typename T>
class FlagEvaluationDetails : public ResolutionDetails<T> {
 public:
  FlagEvaluationDetails(
      std::string flag_key, T value, Reason reason,
      std::optional<std::string> variant, const FlagMetadata& flag_metadata,
      std::optional<ErrorCode> error_code = std::nullopt,
      std::optional<std::string> error_message = std::nullopt);

  FlagEvaluationDetails(std::string flag_key,
                        const ResolutionDetails<T>& resolution_details);

  FlagEvaluationDetails(const FlagEvaluationDetails&) = default;
  FlagEvaluationDetails& operator=(const FlagEvaluationDetails&) = default;
  FlagEvaluationDetails(FlagEvaluationDetails&&) noexcept = default;
  FlagEvaluationDetails& operator=(FlagEvaluationDetails&&) noexcept = default;
  ~FlagEvaluationDetails() = default;
  std::string_view GetFlagKey() const;

 private:
  std::string flag_key_;
};

// Type aliases for common types.
using BoolFlagEvaluationDetails = FlagEvaluationDetails<bool>;
using StringFlagEvaluationDetails = FlagEvaluationDetails<std::string>;
using IntFlagEvaluationDetails = FlagEvaluationDetails<int64_t>;
using DoubleFlagEvaluationDetails = FlagEvaluationDetails<double>;
using ObjectFlagEvaluationDetails = FlagEvaluationDetails<Value>;

}  // namespace openfeature

#endif  // CPP_SDK_INCLUDE_OPENFEATURE_FLAG_EVALUATION_DETAILS_H_
