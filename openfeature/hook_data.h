#ifndef CPP_SDK_INCLUDE_OPENFEATURE_HOOK_DATA_H_
#define CPP_SDK_INCLUDE_OPENFEATURE_HOOK_DATA_H_

#include <any>
#include <string>
#include <unordered_map>

namespace openfeature {

// HookData provides a way for hooks to maintain state across their execution
// stages. Each hook instance gets its own isolated data store that persists
// only for the duration of a single flag evaluation.
class HookData {
 public:
  HookData() = default;

  void Set(std::string key, std::any value);

  const std::any* Get(const std::string& key) const;

  template <typename T>
  T* GetAs(const std::string& key) {
    auto it_key = data_.find(key);
    if (it_key != data_.end()) {
      return std::any_cast<T>(&it_key->second);
    }
    return nullptr;
  }

 private:
  std::unordered_map<std::string, std::any> data_;
};

}  // namespace openfeature
#endif  // CPP_SDK_INCLUDE_OPENFEATURE_HOOK_DATA_H_
