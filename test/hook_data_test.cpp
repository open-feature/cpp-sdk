#include "openfeature/hook_data.h"

#include <gtest/gtest.h>

#include <any>
#include <memory>
#include <string>

namespace openfeature {
namespace {

constexpr char kNonExistentKey[] = "non_existent_key";

constexpr char kIntKey[] = "int_attr";
constexpr int kIntValue = 42;

constexpr char kStringKey[] = "string_attr";
constexpr char kStringValue[] = "hello world";

constexpr char kBoolKey[] = "bool_attr";
constexpr bool kBoolValue = true;

constexpr char kDoubleKey[] = "double_attr";
constexpr double kDoubleValue = 3.14159;

constexpr char kCounterKey[] = "counter";
constexpr int kInitialCounterValue = 100;
constexpr int kUpdatedCounterValue = 200;

constexpr char kDynamicKey[] = "dynamic_key";
constexpr int kDynamicIntValue = 12345;
constexpr char kDynamicStringValue[] = "now a string";

constexpr char kMyIntKey[] = "my_int";
constexpr int kMyIntValue = 99;

constexpr char kCustomStateKey[] = "custom_state";
constexpr int kInitialCallCount = 1;
constexpr char kBeforeStageName[] = "before_stage";

constexpr char kStageTrackerKey[] = "stage_tracker";
constexpr char kBeforeStage[] = "before";
constexpr char kAfterStage[] = "after";
constexpr int kUpdatedCallCount = 2;

constexpr char kSharedKey[] = "shared_key";
constexpr char kSharedDataValue[] = "shared data";
constexpr int kExpectedUseCount = 2;

class HookDataTest : public ::testing::Test {
 protected:
  HookData hook_data_;
};

TEST_F(HookDataTest, DefaultConstructorCreatesEmptyData) {
  EXPECT_EQ(hook_data_.Get(kNonExistentKey), nullptr);
  EXPECT_EQ(hook_data_.GetAs<int>(kNonExistentKey), nullptr);
}

TEST_F(HookDataTest, SetAndGetPrimitiveTypes) {
  hook_data_.Set(kIntKey, kIntValue);
  hook_data_.Set(kStringKey, std::string(kStringValue));
  hook_data_.Set(kBoolKey, kBoolValue);
  hook_data_.Set(kDoubleKey, kDoubleValue);

  const std::any* int_any = hook_data_.Get(kIntKey);
  ASSERT_NE(int_any, nullptr);
  EXPECT_EQ(std::any_cast<int>(*int_any), kIntValue);

  int* int_ptr = hook_data_.GetAs<int>(kIntKey);
  ASSERT_NE(int_ptr, nullptr);
  EXPECT_EQ(*int_ptr, kIntValue);

  std::string* str_ptr = hook_data_.GetAs<std::string>(kStringKey);
  ASSERT_NE(str_ptr, nullptr);
  EXPECT_EQ(*str_ptr, kStringValue);

  bool* bool_ptr = hook_data_.GetAs<bool>(kBoolKey);
  ASSERT_NE(bool_ptr, nullptr);
  EXPECT_EQ(*bool_ptr, kBoolValue);

  double* double_ptr = hook_data_.GetAs<double>(kDoubleKey);
  ASSERT_NE(double_ptr, nullptr);
  EXPECT_DOUBLE_EQ(*double_ptr, kDoubleValue);
}

TEST_F(HookDataTest, SetOverwritesExistingKeyWithSameType) {
  hook_data_.Set(kCounterKey, kInitialCounterValue);
  ASSERT_NE(hook_data_.GetAs<int>(kCounterKey), nullptr);
  EXPECT_EQ(*hook_data_.GetAs<int>(kCounterKey), kInitialCounterValue);

  hook_data_.Set(kCounterKey, kUpdatedCounterValue);
  ASSERT_NE(hook_data_.GetAs<int>(kCounterKey), nullptr);
  EXPECT_EQ(*hook_data_.GetAs<int>(kCounterKey), kUpdatedCounterValue);
}

TEST_F(HookDataTest, SetOverwritesExistingKeyWithDifferentType) {
  hook_data_.Set(kDynamicKey, kDynamicIntValue);
  EXPECT_NE(hook_data_.GetAs<int>(kDynamicKey), nullptr);

  hook_data_.Set(kDynamicKey, std::string(kDynamicStringValue));
  EXPECT_EQ(hook_data_.GetAs<int>(kDynamicKey), nullptr);

  std::string* str_ptr = hook_data_.GetAs<std::string>(kDynamicKey);
  ASSERT_NE(str_ptr, nullptr);
  EXPECT_EQ(*str_ptr, kDynamicStringValue);
}

TEST_F(HookDataTest, GetAsReturnsNullptrOnTypeMismatch) {
  hook_data_.Set(kMyIntKey, kMyIntValue);

  EXPECT_EQ(hook_data_.GetAs<std::string>(kMyIntKey), nullptr);
  EXPECT_EQ(hook_data_.GetAs<double>(kMyIntKey), nullptr);
  EXPECT_EQ(hook_data_.GetAs<bool>(kMyIntKey), nullptr);
}

// Custom struct for testing user-defined type storage across hooks.
struct CustomEvaluationState {
  int call_count;
  std::string step_name;
};

TEST_F(HookDataTest, SetAndGetCustomStruct) {
  CustomEvaluationState state{kInitialCallCount, kBeforeStageName};
  hook_data_.Set(kCustomStateKey, state);

  CustomEvaluationState* retrieved =
      hook_data_.GetAs<CustomEvaluationState>(kCustomStateKey);
  ASSERT_NE(retrieved, nullptr);
  EXPECT_EQ(retrieved->call_count, kInitialCallCount);
  EXPECT_EQ(retrieved->step_name, kBeforeStageName);
}

TEST_F(HookDataTest, ModifyStoredValueInPlaceViaGetAs) {
  hook_data_.Set(kStageTrackerKey,
                 CustomEvaluationState{kInitialCallCount, kBeforeStage});

  CustomEvaluationState* state_ptr =
      hook_data_.GetAs<CustomEvaluationState>(kStageTrackerKey);
  ASSERT_NE(state_ptr, nullptr);
  state_ptr->call_count++;
  state_ptr->step_name = kAfterStage;

  CustomEvaluationState* updated_ptr =
      hook_data_.GetAs<CustomEvaluationState>(kStageTrackerKey);
  ASSERT_NE(updated_ptr, nullptr);
  EXPECT_EQ(updated_ptr->call_count, kUpdatedCallCount);
  EXPECT_EQ(updated_ptr->step_name, kAfterStage);
}

TEST_F(HookDataTest, SetAndGetSharedPtr) {
  auto ptr = std::make_shared<std::string>(kSharedDataValue);
  hook_data_.Set(kSharedKey, ptr);

  auto* retrieved = hook_data_.GetAs<std::shared_ptr<std::string>>(kSharedKey);
  ASSERT_NE(retrieved, nullptr);
  ASSERT_NE(*retrieved, nullptr);
  EXPECT_EQ(**retrieved, kSharedDataValue);
  EXPECT_EQ(retrieved->use_count(), kExpectedUseCount);
}

}  // namespace
}  // namespace openfeature
