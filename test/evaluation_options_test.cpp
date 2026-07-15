#include "openfeature/evaluation_options.h"

#include <gtest/gtest.h>

#include <any>
#include <memory>
#include <string>
#include <vector>

#include "openfeature/base_hook.h"
#include "openfeature/hook.h"
#include "openfeature/hook_hints.h"

namespace openfeature {

class CustomTestHook : public BoolHook {};
class AnotherTestHook : public StringHook {};

TEST(EvaluationOptionsTest, DefaultConstructorInitializesEmptyContainers) {
  EvaluationOptions options;
  EXPECT_TRUE(options.hooks.empty());
  EXPECT_TRUE(options.hook_hints.empty());
}

TEST(EvaluationOptionsTest, StoresAndPreservesMultipleHooksInOrder) {
  EvaluationOptions options;

  auto hook1 = std::make_shared<CustomTestHook>();
  auto hook2 = std::make_shared<AnotherTestHook>();
  auto hook3 = std::make_shared<IntHook>();

  options.hooks.push_back(hook1);
  options.hooks.push_back(hook2);
  options.hooks.push_back(hook3);

  ASSERT_EQ(options.hooks.size(), 3);
  EXPECT_EQ(options.hooks[0], hook1);
  EXPECT_EQ(options.hooks[1], hook2);
  EXPECT_EQ(options.hooks[2], hook3);
}

TEST(EvaluationOptionsTest, CanDowncastBaseHookPointersToConcreteTypes) {
  EvaluationOptions options;

  auto bool_hook = std::make_shared<CustomTestHook>();
  auto string_hook = std::make_shared<AnotherTestHook>();

  options.hooks.push_back(bool_hook);
  options.hooks.push_back(string_hook);

  ASSERT_EQ(options.hooks.size(), 2);

  auto retrieved_bool_hook =
      std::dynamic_pointer_cast<BoolHook>(options.hooks[0]);
  ASSERT_NE(retrieved_bool_hook, nullptr);
  EXPECT_EQ(retrieved_bool_hook, bool_hook);

  auto retrieved_string_hook =
      std::dynamic_pointer_cast<StringHook>(options.hooks[1]);
  ASSERT_NE(retrieved_string_hook, nullptr);
  EXPECT_EQ(retrieved_string_hook, string_hook);

  // Cross-type downcast should return nullptr safely
  auto invalid_cast = std::dynamic_pointer_cast<IntHook>(options.hooks[0]);
  EXPECT_EQ(invalid_cast, nullptr);
}

TEST(EvaluationOptionsTest, StoresAndRetrievesHookHints) {
  const std::string trace_id_key = "trace_id";
  const std::string timeout_key = "timeout_ms";
  const std::string is_debug_key = "is_debug";
  const std::string trace_id_value = "trace-12345";
  const int timeout_value = 500;
  const bool is_debug_value = true;
  EvaluationOptions options;

  options.hook_hints[trace_id_key] = std::string(trace_id_value);
  options.hook_hints[timeout_key] = timeout_value;
  options.hook_hints[is_debug_key] = is_debug_value;

  EXPECT_EQ(options.hook_hints.size(), 3);

  auto it_trace = options.hook_hints.find(trace_id_key);
  ASSERT_NE(it_trace, options.hook_hints.end());
  EXPECT_EQ(std::any_cast<std::string>(it_trace->second), trace_id_value);

  auto it_timeout = options.hook_hints.find(timeout_key);
  ASSERT_NE(it_timeout, options.hook_hints.end());
  EXPECT_EQ(std::any_cast<int>(it_timeout->second), timeout_value);

  auto it_debug = options.hook_hints.find(is_debug_key);
  ASSERT_NE(it_debug, options.hook_hints.end());
  EXPECT_TRUE(std::any_cast<bool>(it_debug->second));
}

TEST(EvaluationOptionsTest, SupportsCopyAndMoveSemantics) {
  const std::string trace_id_key = "trace_id";
  const std::string trace_id_value = "value";
  EvaluationOptions original;
  auto hook = std::make_shared<CustomTestHook>();
  original.hooks.push_back(hook);
  original.hook_hints[trace_id_key] = trace_id_value;

  // Copy construction
  EvaluationOptions copy_constructed(original);
  EXPECT_EQ(copy_constructed.hooks.size(), 1);
  EXPECT_EQ(copy_constructed.hooks[0], hook);
  EXPECT_EQ(std::any_cast<std::string>(copy_constructed.hook_hints[trace_id_key]),
            trace_id_value);

  // Move construction
  EvaluationOptions moved_constructed(std::move(original));
  EXPECT_EQ(moved_constructed.hooks.size(), 1);
  EXPECT_EQ(moved_constructed.hooks[0], hook);
  EXPECT_EQ(std::any_cast<std::string>(moved_constructed.hook_hints[trace_id_key]),
            trace_id_value);
}

}  // namespace openfeature
