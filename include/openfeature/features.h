#ifndef CPP_SDK_INCLUDE_OPENFEATURE_FEATURES_H_
#define CPP_SDK_INCLUDE_OPENFEATURE_FEATURES_H_

#include <string_view>

#include "openfeature/evaluation_context.h"

namespace openfeature {

class Features {
 public:
  virtual ~Features() = default;
  virtual bool GetBooleanValue(std::string_view flag_key,
                               bool default_value) = 0;
  virtual bool GetBooleanValue(std::string_view flag_key, bool default_value,
                               const EvaluationContext& ctx) = 0;

  // TODO: Add other flag types (e.g. string, int, float, object)
  // TODO: Add detailed evaluation methods
};

}  // namespace openfeature

#endif CPP_SDK_INCLUDE_OPENFEATURE_FEATURES_H_
