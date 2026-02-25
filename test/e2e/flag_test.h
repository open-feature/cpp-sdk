#ifndef CPP_SDK_INCLUDE_TEST_E2E_FLAG_TEST_H_
#define CPP_SDK_INCLUDE_TEST_E2E_FLAG_TEST_H_

#include <any>
#include <string>

namespace openfeature_e2e {

struct FlagTest {
  std::string type;
  std::string name;
  std::any default_value;
};

}  // namespace openfeature_e2e
#endif  // CPP_SDK_INCLUDE_TEST_E2E_FLAG_TEST_H_