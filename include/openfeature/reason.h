#ifndef CPP_SDK_INCLUDE_OPENFEATURE_REASON_H_
#define CPP_SDK_INCLUDE_OPENFEATURE_REASON_H_ 

#include <string>

namespace openfeature {

// Represents the reason for a particular flag evaluation result.
//
// The Reason enum provides a standardized explanation for the
// value returned by a flag evaluation.
enum class Reason{
    kStatic,          // The resolved value is static (no dynamic evaluation).
    kDefault,         // The resolved value fell back to a pre-configured value (no dynamic evaluation occurred or dynamic evaluation yielded no result).
    kTargetingMatch, // The resolved value was the result of a dynamic evaluation, such as a rule or specific user-targeting.
    kSplit,           // The resolved value was the result of pseudorandom assignment.
    kCached,          // The resolved value was retrieved from a cache.
    kDisabled,        // The resolved value was the result of the flag being disabled in the management system.
    kUnknown,         // The reason for the resolved value could not be determined.
    kStale,           // The resolved value is non-authoritative or possibly out of date
    kError            // The resolved value was the result of an error.
};

// TODO: Implement a function to convert Reason to string representation if needed.

} // namespace openfeature

#endif CPP_SDK_INCLUDE_OPENFEATURE_REASON_H_