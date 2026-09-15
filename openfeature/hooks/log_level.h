#ifndef CPP_SDK_INCLUDE_OPENFEATURE_HOOKS_LOG_LEVEL_H_
#define CPP_SDK_INCLUDE_OPENFEATURE_HOOKS_LOG_LEVEL_H_

#include <cstdint>

namespace openfeature {

// Represents severity levels for logging flag evaluation events.
enum class LogLevel : std::uint8_t { kDebug, kInfo, kWarn, kError };

}  // namespace openfeature

#endif  // CPP_SDK_INCLUDE_OPENFEATURE_HOOKS_LOG_LEVEL_H_
