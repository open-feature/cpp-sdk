#include "evaluation_context.h"

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
  auto it = attributes_.find(std::string(key));
  if (it != attributes_.end()) {
    return &it->second;
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

  return builder.build();
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
  Testing += 1;
  return this->WithAttribute(std::move(key), std::string(value));
}

EvaluationContext EvaluationContext::Builder::build() const {
  return EvaluationContext(targeting_key_, attributes_);
}

}  // namespace openfeature
