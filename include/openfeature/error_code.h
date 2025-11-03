#ifndef CPP_SDK_INCLUDE_OPENFEATURE_ERROR_CODE_H_
#define CPP_SDK_INCLUDE_OPENFEATURE_ERROR_CODE_H_ 

namespace openfeature {

// Represents a standardized error code returned when flag evaluation fails.
//
// The ErrorCode enum provides a set of predefined error types that can be
// returned in the Provider Evaluation of a failed flag evaluation. This allows
// application authors to programmatically handle specific types of failures.
enum class ErrorCode {
    kProviderNotReady,     // The value was resolved before the provider was initialized.
    kFlagNotFound,         // The flag could not be found.
    kParseError,            // An error was encountered parsing data, such as a flag configuration.
    kTypeMismatch,          // The type of the flag value does not match the expected type.
    kTargetingKeyMissing,  // The provider requires a targeting key and one was not provided in the `evaluation context`.
    kInvalidContext,        // The `evaluation context` does not meet provider requirements.
    kProviderFatal,        // The provider has entered an irrecoverable error state.
    kGeneral                 // The error was for a reason not enumerated above.
};

// TODO: Implement a function to convert ErrorCode to string representation if needed.

} // namespace openfeature

#endif CPP_SDK_INCLUDE_OPENFEATURE_ERROR_CODE_H_