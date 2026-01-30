#ifndef CPP_SDK_INCLUDE_OPENFEATURE_EVALUATION_CONTEXT_H_
#define CPP_SDK_INCLUDE_OPENFEATURE_EVALUATION_CONTEXT_H_

#include <algorithm>
#include <any>
#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace openfeature {

// EvaluationContext provides information for the purposes of flag
// evaluation.
class EvaluationContext {
 public:
  // The Builder class is the only way to construct an EvaluationContext.
  class Builder;

  EvaluationContext() = delete;

  std::optional<std::string_view> GetTargetingKey() const;

  // Get a specific attribute value.
  // Returns nullptr if key does not exist.
  const std::any* GetValue(std::string_view key) const;

  // Get all attributes
  const std::map<std::string, std::any>& GetAttributes() const;

  // It takes a list of pointers to contexts to avoid unnecessary copies.
  // The order of contexts in the initializer list determines precedence.
  static EvaluationContext merge(
      std::initializer_list<const EvaluationContext*> contexts);

 private:
  EvaluationContext(std::optional<std::string> targeting_key,
                    std::map<std::string, std::any> attributes);

  std::optional<std::string> targeting_key_;
  std::map<std::string, std::any> attributes_;
};

class EvaluationContext::Builder {
 public:
  // Builder methods return a reference to self to allow for chaining.
  Builder& withTargetingKey(std::string key);
  Builder& withAttribute(std::string key, std::any value);
  // Overload for const char* to ensure implicit conversion to std::string
  Builder& withAttribute(std::string key, const char* value);

  // The build() method creates the final, immutable EvaluationContext object.
  EvaluationContext build() const;

 private:
  std::optional<std::string> targeting_key_;
  std::map<std::string, std::any> attributes_;
};

}  // namespace openfeature

#endif  // CPP_SDK_INCLUDE_OPENFEATURE_EVALUATION_CONTEXT_H_
