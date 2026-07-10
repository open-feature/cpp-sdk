#include "openfeature/hook_data.h"

#include <any>
#include <string>
#include <unordered_map>

namespace openfeature {

void HookData::Set(std::string key, std::any value) {
  data_.insert_or_assign(std::move(key), std::move(value));
}

const std::any* HookData::Get(const std::string& key) const {
  auto it_key = data_.find(key);
  if (it_key != data_.end()) {
    return &it_key->second;
  }
  return nullptr;
}

}  // namespace openfeature
