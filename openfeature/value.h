#ifndef CPP_SDK_INCLUDE_OPENFEATURE_VALUE_H_
#define CPP_SDK_INCLUDE_OPENFEATURE_VALUE_H_

#include <chrono>
#include <map>
#include <memory>
#include <optional>
#include <ostream>
#include <string>
#include <variant>
#include <vector>

namespace openfeature {

// Value serves as a generic return type for structured data from providers.
// It can hold Null, Boolean, String, Integer, Double, Structure (Map), List, or
// DateTime.
class Value {
 public:
  Value();
  explicit Value(bool value);
  explicit Value(int64_t value);
  explicit Value(int value);
  explicit Value(double value);
  explicit Value(std::string value);
  explicit Value(const char* value);
  explicit Value(const std::map<std::string, Value>& value);
  explicit Value(const std::vector<Value>& value);
  explicit Value(std::chrono::system_clock::time_point value);

  Value(const Value& other);
  Value& operator=(const Value& other);
  Value(Value&& other) noexcept = default;
  Value& operator=(Value&& other) noexcept = default;
  ~Value() = default;

  bool IsNull() const;
  bool IsBool() const;
  bool IsNumber() const;
  bool IsString() const;
  bool IsStructure() const;
  bool IsList() const;
  bool IsDateTime() const;

  // Returns generic optional, or nullopt if type mismatch
  std::optional<bool> AsBool() const;
  std::optional<std::string> AsString() const;
  std::optional<int64_t> AsInt() const;
  std::optional<double> AsDouble() const;
  std::optional<std::chrono::system_clock::time_point> AsDateTime() const;

  // For complex types, return pointers to avoid expensive copies on access
  // Returns nullptr if type mismatch
  const std::map<std::string, Value>* AsStructure() const;
  const std::vector<Value>* AsList() const;

  // Returns a JSON-formatted or human-readable string representation of the
  // value.
  std::string ToString() const;

 private:
  using InternalVariant =
      std::variant<std::monostate, bool, int64_t, double, std::string,
                   std::chrono::system_clock::time_point,
                   std::unique_ptr<std::vector<Value>>,
                   std::unique_ptr<std::map<std::string, Value>>>;

  InternalVariant inner_value_;
};

bool operator==(const Value& lhs, const Value& rhs);
bool operator!=(const Value& lhs, const Value& rhs);
std::ostream& operator<<(std::ostream& output_stream, const Value& value);

}  // namespace openfeature

#endif  // CPP_SDK_INCLUDE_OPENFEATURE_VALUE_H_
