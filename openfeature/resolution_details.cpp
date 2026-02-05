#include "openfeature/resolution_details.h"

namespace openfeature {

template <typename T>
ResolutionDetails<T>::ResolutionDetails(
    T value, Reason reason, std::optional<std::string> variant,
    const FlagMetadata& flag_metadata, std::optional<ErrorCode> error_code,
    std::optional<std::string> error_message)
    : value_(std::move(value)),
      error_code_(error_code),
      error_message_(std::move(error_message)),
      reason_(reason),
      variant_(std::move(variant)),
      flag_metadata_(flag_metadata) {}

template <typename T>
T ResolutionDetails<T>::GetValue() const {
  return value_;
}

template <typename T>
std::optional<std::string> ResolutionDetails<T>::GetVariant() const {
  return variant_;
}

template <typename T>
Reason ResolutionDetails<T>::GetReason() const {
  return reason_;
}

template <typename T>
std::optional<ErrorCode> ResolutionDetails<T>::GetErrorCode() const {
  return error_code_;
}

template <typename T>
std::optional<std::string> ResolutionDetails<T>::GetErrorMessage() const {
  return error_message_;
}

template <typename T>
const FlagMetadata& ResolutionDetails<T>::GetFlagMetadata() const {
  return flag_metadata_;
}

template class ResolutionDetails<bool>;
template class ResolutionDetails<std::string>;
template class ResolutionDetails<int64_t>;
template class ResolutionDetails<double>;
template class ResolutionDetails<Value>;

}  // namespace openfeature