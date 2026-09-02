#include "evaluation_context.h"

#include <ostream>
#include <sstream>

#include "openfeature/value.h"

namespace openfeature {

EvaluationContext::EvaluationContext(std::optional<std::string> targeting_key,
                                     std::map<std::string, std::any> attributes)
    : targeting_key_(std::move(targeting_key)),
      attributes_(std::move(attributes)) {}

std::optional<std::string_view> EvaluationContext::GetTargetingKey() const {
  if (!targeting_key_.has_value()) {
    return std::nullopt;
  }
  return targeting_key_;
}

const std::any* EvaluationContext::GetValue(std::string_view key) const {
  auto iterator = attributes_.find(std::string(key));
  if (iterator != attributes_.end()) {
    return &iterator->second;
  }
  return nullptr;
}

const std::map<std::string, std::any>& EvaluationContext::GetAttributes()
    const {
  return attributes_;
}

EvaluationContext EvaluationContext::Merge(
    std::initializer_list<const EvaluationContext*> contexts) {
  Builder builder;

  // Merge Attributes from all contexts (higher precedence overwrites lower).
  for (const EvaluationContext* ctx : contexts) {
    if (ctx != nullptr) {
      for (const auto& pair : ctx->GetAttributes()) {
        builder.WithAttribute(pair.first, pair.second);
      }
    }
  }

  // Find the first valid targeting key from highest to lowest precedence.
  // We iterate through the list backwards to check the highest precedence
  // context first.
  std::vector<const EvaluationContext*> reversed(contexts);
  std::reverse(reversed.begin(), reversed.end());

  for (const EvaluationContext* ctx : reversed) {
    if (ctx != nullptr) {
      auto key = ctx->GetTargetingKey();
      if (key.has_value() && !key->empty()) {
        builder.WithTargetingKey(std::string(key.value()));
        break;
      }
    }
  }

  return builder.Build();
}

std::string EvaluationContext::ToString() const {
  std::ostringstream string_stream;
  string_stream << "{";
  bool first = true;
  if (auto key = GetTargetingKey(); key.has_value()) {
    string_stream << R"("targeting_key": ")" << *key << R"(")";
    first = false;
  }
  for (const auto& [attr_key, attr_value] : GetAttributes()) {
    if (!first) string_stream << ", ";
    first = false;
    string_stream << "\"" << attr_key << "\": ";
    if (attr_value.type() == typeid(std::string)) {
      string_stream << "\"" << std::any_cast<std::string>(attr_value) << "\"";
    } else if (attr_value.type() == typeid(bool)) {
      string_stream << (std::any_cast<bool>(attr_value) ? "true" : "false");
    } else if (attr_value.type() == typeid(int)) {
      string_stream << std::any_cast<int>(attr_value);
    } else if (attr_value.type() == typeid(int64_t)) {
      string_stream << std::any_cast<int64_t>(attr_value);
    } else if (attr_value.type() == typeid(double)) {
      string_stream << std::any_cast<double>(attr_value);
    } else if (attr_value.type() == typeid(Value)) {
      string_stream << std::any_cast<Value>(attr_value);
    } else {
      string_stream << "\"<any>\"";
    }
  }
  string_stream << "}";
  return string_stream.str();
}

std::ostream& operator<<(std::ostream& output_stream,
                         const EvaluationContext& ctx) {
  return output_stream << ctx.ToString();
}

EvaluationContext::Builder& EvaluationContext::Builder::WithTargetingKey(
    std::string key) {
  this->targeting_key_ = std::move(key);
  return *this;
}

EvaluationContext::Builder& EvaluationContext::Builder::WithAttribute(
    std::string key, std::any value) {
  this->attributes_.insert_or_assign(std::move(key), std::move(value));
  return *this;
}

EvaluationContext::Builder& EvaluationContext::Builder::WithAttribute(
    std::string key, const char* value) {
  return this->WithAttribute(std::move(key), std::string(value));
}

EvaluationContext EvaluationContext::Builder::Build() const {
  return {targeting_key_, attributes_};
}

}  // namespace openfeature
