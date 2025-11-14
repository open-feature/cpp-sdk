#ifndef CPP_SDK_INCLUDE_OPENFEATURE_FLAG_METADATA_H_
#define CPP_SDK_INCLUDE_OPENFEATURE_FLAG_METADATA_H_

#include <string>
#include <unordered_map>
#include <variant>

namespace openfeature {

using FlagMetadataValue = std::variant<bool, std::string, double, int64_t>;

// A structure which supports the definition of arbitrary properties,
// with keys of type string, and values of type boolean, string, or number.
//
// This structure is populated by a provider for use by an Application Author
// (via the Evaluation API) or an Application Integrator (via hooks).
struct FlagMetadata {
  std::unordered_map<std::string, FlagMetadataValue> data;
};

}  // namespace openfeature

#endif CPP_SDK_INCLUDE_OPENFEATURE_FLAG_METADATA_H_
