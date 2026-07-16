#include "openfeature/flag_evaluation_details.h"

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
FlagEvaluationDetails<T>::FlagEvaluationDetails(
    std::string flag_key, T value, Reason reason,
    std::optional<std::string> variant, const FlagMetadata& flag_metadata,
    std::optional<ErrorCode> error_code,
    std::optional<std::string> error_message)
    : ResolutionDetails<T>(std::move(value), reason, std::move(variant),
                           flag_metadata, error_code, std::move(error_message)),
      flag_key_(std::move(flag_key)) {}

template <typename T>
FlagEvaluationDetails<T>::FlagEvaluationDetails(
    std::string flag_key, const ResolutionDetails<T>& resolution_details)
    : ResolutionDetails<T>(resolution_details),
      flag_key_(std::move(flag_key)) {}

template <typename T>
std::string_view FlagEvaluationDetails<T>::GetFlagKey() const {
  return flag_key_;
}

template class FlagEvaluationDetails<bool>;
template class FlagEvaluationDetails<std::string>;
template class FlagEvaluationDetails<int64_t>;
template class FlagEvaluationDetails<double>;
template class FlagEvaluationDetails<Value>;

}  // namespace openfeature
