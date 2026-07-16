#ifndef CPP_SDK_INCLUDE_OPENFEATURE_FLAG_TYPE_VALUE_H_
#define CPP_SDK_INCLUDE_OPENFEATURE_FLAG_TYPE_VALUE_H_

#include <cstdint>

namespace openfeature {

enum class FlagValueType : std::uint8_t {
  kBoolean,
  kString,
  kInteger,
  kDouble,
  kObject
};

}  // namespace openfeature
#endif  // CPP_SDK_INCLUDE_OPENFEATURE_FLAG_TYPE_VALUE_H_
