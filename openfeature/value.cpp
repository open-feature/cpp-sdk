#include "openfeature/value.h"

#include <cmath>

namespace openfeature {

Value::Value() : inner_value_(std::monostate{}) {}

Value::Value(bool value) : inner_value_(value) {}

Value::Value(int64_t value) : inner_value_(value) {}
Value::Value(int value) : inner_value_(static_cast<int64_t>(value)) {}

Value::Value(double value) : inner_value_(value) {}

Value::Value(std::string value) : inner_value_(std::move(value)) {}

Value::Value(const char* value) : inner_value_(std::string(value)) {}

Value::Value(const std::map<std::string, Value>& value)
    : inner_value_(std::make_unique<std::map<std::string, Value>>(value)) {}

Value::Value(const std::vector<Value>& value)
    : inner_value_(std::make_unique<std::vector<Value>>(value)) {}

Value::Value(std::chrono::system_clock::time_point value)
    : inner_value_(value) {}

Value::Value(const Value& other) {
  if (other.IsStructure()) {
    auto& ptr = std::get<std::unique_ptr<std::map<std::string, Value>>>(
        other.inner_value_);
    inner_value_ =
        ptr ? std::make_unique<std::map<std::string, Value>>(*ptr) : nullptr;
  } else if (other.IsList()) {
    auto& ptr =
        std::get<std::unique_ptr<std::vector<Value>>>(other.inner_value_);
    inner_value_ = ptr ? std::make_unique<std::vector<Value>>(*ptr) : nullptr;
  } else {
    std::visit(
        [this](auto&& arg) {
          using T = std::decay_t<decltype(arg)>;
          if constexpr (!std::is_same_v<
                            T, std::unique_ptr<std::map<std::string, Value>>> &&
                        !std::is_same_v<T,
                                        std::unique_ptr<std::vector<Value>>>) {
            this->inner_value_ = arg;
          }
        },
        other.inner_value_);
  }
}

Value& Value::operator=(const Value& other) {
  if (this == &other) return *this;
  Value temp(other);
  std::swap(this->inner_value_, temp.inner_value_);
  return *this;
}

bool Value::IsNull() const {
  return std::holds_alternative<std::monostate>(inner_value_);
}
bool Value::IsBool() const {
  return std::holds_alternative<bool>(inner_value_);
}
bool Value::IsNumber() const {
  return std::holds_alternative<int64_t>(inner_value_) ||
         std::holds_alternative<double>(inner_value_);
}
bool Value::IsString() const {
  return std::holds_alternative<std::string>(inner_value_);
}
bool Value::IsStructure() const {
  return std::holds_alternative<std::unique_ptr<std::map<std::string, Value>>>(
      inner_value_);
}
bool Value::IsList() const {
  return std::holds_alternative<std::unique_ptr<std::vector<Value>>>(
      inner_value_);
}
bool Value::IsDateTime() const {
  return std::holds_alternative<std::chrono::system_clock::time_point>(
      inner_value_);
}

std::optional<bool> Value::AsBool() const {
  if (auto* v = std::get_if<bool>(&inner_value_)) return *v;
  return std::nullopt;
}

std::optional<std::string> Value::AsString() const {
  if (auto* v = std::get_if<std::string>(&inner_value_)) return *v;
  return std::nullopt;
}

std::optional<int64_t> Value::AsInt() const {
  if (auto* v = std::get_if<int64_t>(&inner_value_)) return *v;
  if (auto* v = std::get_if<double>(&inner_value_))
    return static_cast<int64_t>(std::round(*v));
  return std::nullopt;
}

std::optional<double> Value::AsDouble() const {
  if (auto* v = std::get_if<double>(&inner_value_)) return *v;
  if (auto* v = std::get_if<int64_t>(&inner_value_))
    return static_cast<double>(*v);
  return std::nullopt;
}

std::optional<std::chrono::system_clock::time_point> Value::AsDateTime() const {
  if (auto* v =
          std::get_if<std::chrono::system_clock::time_point>(&inner_value_))
    return *v;
  return std::nullopt;
}

const std::map<std::string, Value>* Value::AsStructure() const {
  if (auto* v = std::get_if<std::unique_ptr<std::map<std::string, Value>>>(
          &inner_value_)) {
    return v->get();
  }
  return nullptr;
}

const std::vector<Value>* Value::AsList() const {
  if (auto* v =
          std::get_if<std::unique_ptr<std::vector<Value>>>(&inner_value_)) {
    return v->get();
  }
  return nullptr;
}

bool operator==(const Value& lhs, const Value& rhs) {
  if (lhs.IsBool() && rhs.IsBool()) return lhs.AsBool() == rhs.AsBool();

  if (lhs.IsString() && rhs.IsString()) return lhs.AsString() == rhs.AsString();

  if (lhs.IsNumber() && rhs.IsNumber()) return lhs.AsDouble() == rhs.AsDouble();

  if (lhs.IsNull() && rhs.IsNull()) return true;

  if (lhs.IsDateTime() && rhs.IsDateTime())
    return lhs.AsDateTime() == rhs.AsDateTime();

  if (lhs.IsStructure() && rhs.IsStructure()) {
    return *lhs.AsStructure() == *rhs.AsStructure();
  }
  if (lhs.IsList() && rhs.IsList()) {
    return *lhs.AsList() == *rhs.AsList();
  }

  return false;
}

bool operator!=(const Value& lhs, const Value& rhs) { return !(lhs == rhs); }

}  // namespace openfeature