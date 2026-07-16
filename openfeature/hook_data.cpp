#include "openfeature/hook_data.h"

#include <any>
#include <string>
#include <unordered_map>

namespace openfeature {

void HookData::Set(std::string_view key, std::any value) {
  data_.insert_or_assign(std::string(key), std::move(value));
}

const std::any* HookData::Get(std::string_view key) const {
  auto it_key = data_.find(std::string(key));
  if (it_key != data_.end()) {
    return &it_key->second;
  }
  return nullptr;
}

}  // namespace openfeature
