#ifndef CPP_SDK_INCLUDE_TEST_E2E_FLAG_H_
#define CPP_SDK_INCLUDE_TEST_E2E_FLAG_H_

#include <any>
#include <string>
#include <utility>

#include "openfeature/client.h"
#include "openfeature/evaluation_context.h"
#include "openfeature/flag_metadata.h"
#include "openfeature/provider.h"
#include "openfeature/resolution_details.h"  // For ResolutionDetails<Value>

namespace openfeature_e2e {

class Flag {
 public:
  const std::string name_;
  const std::any default_value_;
  const std::string type_;
  const openfeature::FlagMetadata flag_metadata_;

  Flag(std::string type, std::string name, std::any default_value)
      : Flag(std::move(type), std::move(name), std::move(default_value),
             openfeature::FlagMetadata{}) {}

  Flag(std::string type, std::string name, std::any default_value,
       openfeature::FlagMetadata flag_metadata)
      : name_(std::move(name)),
        default_value_(std::move(default_value)),
        type_(std::move(type)),
        flag_metadata_(std::move(flag_metadata)) {}
};

}  // namespace openfeature_e2e
#endif  // CPP_SDK_INCLUDE_TEST_E2E_FLAG_H_