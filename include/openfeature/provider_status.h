#ifndef CPP_SDK_INCLUDE_OPENFEATURE_PROVIDER_STATUS_H_
#define CPP_SDK_INCLUDE_OPENFEATURE_PROVIDER_STATUS_H_

namespace openfeature {

enum class ProviderStatus {
    kNotReady,      // The provider has not been initialized.
    kReady,         // The provider has been initialized, and is able to reliably resolve flag values.
    kError,        // The provider is initialized but is not able to reliably resolve flag values.
    kStale,        // The provider's cached state is no longer valid and may not be up-to-date with the source of truth.
    kFatal         // The provider has entered an irrecoverable error state.
};

// TODO: Implement a function to convert ProviderStatus to string representation if needed.

} // namespace openfeature

#endif CPP_SDK_INCLUDE_OPENFEATURE_PROVIDER_STATUS_H_
