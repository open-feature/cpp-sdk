#include "openfeature/value.h"

#include <chrono>
#include <limits>
#include <map>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "gtest/gtest.h"

namespace openfeature {

using namespace std::chrono_literals;

namespace {

// Test constants to avoid magic numbers
constexpr int kTestInt = 456;
constexpr int64_t kTestInt64 = 123LL;
constexpr int64_t kExpectedInt64 = 456LL;
constexpr int kTestVal5 = 5;
constexpr int64_t kExpectedInt5 = 5LL;
constexpr int kTestVal6 = 6;
constexpr int64_t kExpectedInt64_6 = 6LL;
constexpr int kTestVal10 = 10;
constexpr int64_t kExpectedInt10 = 10LL;
constexpr int kTestVal11 = 11;
constexpr int kListItem42 = 42;
constexpr int kTestVal100 = 100;
constexpr int64_t kExpectedInt100 = 100LL;
constexpr int kTestVal200 = 200;
constexpr int64_t kExpectedInt200 = 200LL;
constexpr int64_t kExpectedRoundedInt = 123LL;
constexpr int64_t kRoundUpExpected = 124LL;
constexpr int64_t kRoundDownExpected = 123LL;
constexpr int64_t kExpectedRoundedUp6 = 6LL;
constexpr int64_t kExpectedNegative5 = -5LL;

constexpr double kTestDouble = 123.45;
constexpr double kTestDouble123 = 123.0;
constexpr double kExpectedDouble = 456.0;
constexpr double kRoundUpInput = 123.5;
constexpr double kRoundDownInput = 123.4;
constexpr double kTestDoubleList = 4.5;
constexpr double kTestDoublePi = 3.14;
constexpr double kTestDoublePiOther = 3.15;
constexpr double kTestDouble50 = 5.0;
constexpr double kTestDouble51 = 5.1;
constexpr double kTestDouble57 = 5.7;
constexpr double kNegative53 = -5.3;
constexpr double kNegative55 = -5.5;

constexpr size_t kExpectedListSize = 3;

}  // namespace

TEST(ValueTest, DefaultConstructorIsNull) {
  Value value;
  EXPECT_TRUE(value.IsNull());
  EXPECT_FALSE(value.IsBool());
  EXPECT_FALSE(value.IsNumber());
  EXPECT_FALSE(value.IsString());
  EXPECT_FALSE(value.IsStructure());
  EXPECT_FALSE(value.IsList());
  EXPECT_FALSE(value.IsDateTime());
}

TEST(ValueTest, BoolConstructorAndAccessors) {
  Value true_val(true);
  EXPECT_FALSE(true_val.IsNull());
  EXPECT_TRUE(true_val.IsBool());
  EXPECT_EQ(true_val.AsBool(), true);
  EXPECT_FALSE(true_val.AsInt().has_value());
  EXPECT_FALSE(true_val.AsString().has_value());

  Value false_val(false);
  EXPECT_EQ(false_val.AsBool(), false);
}

TEST(ValueTest, Int64ConstructorAndAccessors) {
  Value int_val(static_cast<int64_t>(kTestInt64));
  EXPECT_FALSE(int_val.IsNull());
  EXPECT_TRUE(int_val.IsNumber());
  EXPECT_EQ(int_val.AsInt(), kTestInt64);
  EXPECT_EQ(int_val.AsDouble(), kTestDouble123);
  EXPECT_FALSE(int_val.AsBool().has_value());

  Value int_min(std::numeric_limits<int64_t>::min());
  EXPECT_EQ(int_min.AsInt(), std::numeric_limits<int64_t>::min());

  Value int_max(std::numeric_limits<int64_t>::max());
  EXPECT_EQ(int_max.AsInt(), std::numeric_limits<int64_t>::max());
}

TEST(ValueTest, IntConstructorAndAccessors) {
  Value int_val(kTestInt);
  EXPECT_FALSE(int_val.IsNull());
  EXPECT_TRUE(int_val.IsNumber());
  EXPECT_EQ(int_val.AsInt(), kExpectedInt64);
  EXPECT_EQ(int_val.AsDouble(), kExpectedDouble);
}

TEST(ValueTest, DoubleConstructorAndAccessors) {
  Value double_val(kTestDouble);
  EXPECT_FALSE(double_val.IsNull());
  EXPECT_TRUE(double_val.IsNumber());
  EXPECT_EQ(double_val.AsDouble(), kTestDouble);
  EXPECT_EQ(double_val.AsInt(), kExpectedRoundedInt);  // Should round
  EXPECT_FALSE(double_val.AsBool().has_value());

  Value double_round_up(kRoundUpInput);
  EXPECT_EQ(double_round_up.AsInt(), kRoundUpExpected);

  Value double_round_down(kRoundDownInput);
  EXPECT_EQ(double_round_down.AsInt(), kRoundDownExpected);
}

TEST(ValueTest, StringConstructorAndAccessors) {
  Value string_val(std::string{"hello"});
  EXPECT_FALSE(string_val.IsNull());
  EXPECT_TRUE(string_val.IsString());
  EXPECT_EQ(string_val.AsString(), "hello");
  EXPECT_FALSE(string_val.AsBool().has_value());

  Value c_string_val("world");
  EXPECT_TRUE(c_string_val.IsString());
  EXPECT_EQ(c_string_val.AsString(), "world");
}

TEST(ValueTest, MapConstructorAndAccessors) {
  std::map<std::string, Value> test_map;
  test_map["key1"] = Value("value1");
  test_map["key2"] = Value(true);
  test_map["key3"] = Value(static_cast<int>(kTestInt64));

  Value map_val(test_map);
  EXPECT_FALSE(map_val.IsNull());
  EXPECT_TRUE(map_val.IsStructure());
  EXPECT_NE(map_val.AsStructure(), nullptr);
  ASSERT_TRUE(map_val.AsStructure()->count("key1"));
  EXPECT_EQ(map_val.AsStructure()->at("key1").AsString(), "value1");
  EXPECT_EQ(map_val.AsStructure()->at("key2").AsBool(), true);
  EXPECT_EQ(map_val.AsStructure()->at("key3").AsInt(), kTestInt64);

  EXPECT_FALSE(map_val.AsBool().has_value());
  EXPECT_EQ(map_val.AsList(), nullptr);

  Value empty_map_val(std::map<std::string, Value>{});
  EXPECT_TRUE(empty_map_val.IsStructure());
  EXPECT_NE(empty_map_val.AsStructure(), nullptr);
  EXPECT_TRUE(empty_map_val.AsStructure()->empty());
}

TEST(ValueTest, VectorConstructorAndAccessors) {
  std::vector<Value> test_list;
  test_list.emplace_back("item1");
  test_list.emplace_back(false);
  test_list.emplace_back(kTestDoubleList);

  Value list_val(test_list);
  EXPECT_FALSE(list_val.IsNull());
  EXPECT_TRUE(list_val.IsList());
  EXPECT_NE(list_val.AsList(), nullptr);
  ASSERT_EQ(list_val.AsList()->size(), kExpectedListSize);
  EXPECT_EQ(list_val.AsList()->at(0).AsString(), "item1");
  EXPECT_EQ(list_val.AsList()->at(1).AsBool(), false);
  EXPECT_EQ(list_val.AsList()->at(2).AsDouble(), kTestDoubleList);

  EXPECT_FALSE(list_val.AsInt().has_value());
  EXPECT_EQ(list_val.AsStructure(), nullptr);

  Value empty_list_val(std::vector<Value>{});
  EXPECT_TRUE(empty_list_val.IsList());
  EXPECT_NE(empty_list_val.AsList(), nullptr);
  EXPECT_TRUE(empty_list_val.AsList()->empty());
}

TEST(ValueTest, DateTimeConstructorAndAccessors) {
  auto now = std::chrono::system_clock::now();
  Value dt_val(now);
  EXPECT_FALSE(dt_val.IsNull());
  EXPECT_TRUE(dt_val.IsDateTime());
  EXPECT_EQ(dt_val.AsDateTime(), now);
  EXPECT_FALSE(dt_val.AsBool().has_value());
}

TEST(ValueTest, CopyConstructor) {
  std::map<std::string, Value> inner_map;
  inner_map["nested_key"] = Value(kTestVal100);
  std::vector<Value> inner_list = {Value("list_item")};

  std::map<std::string, Value> original_map;
  original_map["bool"] = Value(true);
  original_map["int"] = Value(static_cast<int>(kTestInt64));
  original_map["str"] = Value("original");
  original_map["sub_map"] = Value(inner_map);
  original_map["sub_list"] = Value(inner_list);

  Value original_value(original_map);

  Value copied_value = original_value;

  EXPECT_EQ(copied_value, original_value);

  EXPECT_NE(original_value.AsStructure(), nullptr);
  EXPECT_NE(copied_value.AsStructure(), nullptr);

  original_map["bool"] = Value(false);
  original_value = Value(original_map);

  EXPECT_NE(copied_value, original_value);
  EXPECT_EQ(copied_value.AsStructure()->at("bool").AsBool(), true);
  EXPECT_EQ(original_value.AsStructure()->at("bool").AsBool(), false);

  ASSERT_NE(original_value.AsStructure()->at("sub_map").AsStructure(), nullptr);
  ASSERT_NE(copied_value.AsStructure()->at("sub_map").AsStructure(), nullptr);
  EXPECT_EQ(original_value.AsStructure()
                ->at("sub_map")
                .AsStructure()
                ->at("nested_key")
                .AsInt(),
            kExpectedInt100);
  EXPECT_EQ(copied_value.AsStructure()
                ->at("sub_map")
                .AsStructure()
                ->at("nested_key")
                .AsInt(),
            kExpectedInt100);

  std::map<std::string, Value> original_nested_map =
      *original_value.AsStructure()->at("sub_map").AsStructure();
  original_nested_map["nested_key"] = Value(kTestVal200);
  original_map["sub_map"] = Value(original_nested_map);
  original_value = Value(original_map);

  EXPECT_EQ(original_value.AsStructure()
                ->at("sub_map")
                .AsStructure()
                ->at("nested_key")
                .AsInt(),
            kExpectedInt200);
  EXPECT_EQ(copied_value.AsStructure()
                ->at("sub_map")
                .AsStructure()
                ->at("nested_key")
                .AsInt(),
            kExpectedInt100);
}

TEST(ValueTest, AssignmentOperator) {
  Value val1(kTestVal10);
  Value val2("test");
  Value val3;

  val3 = val1;
  EXPECT_EQ(val3.AsInt(), kExpectedInt10);
  EXPECT_EQ(val3, val1);

  val3 = val2;
  EXPECT_EQ(val3.AsString(), "test");
  EXPECT_EQ(val3, val2);

  std::map<std::string, Value> original_map;
  original_map["key"] = Value(kTestVal5);
  Value map_val(original_map);

  Value assigned_map_val;
  assigned_map_val = map_val;
  EXPECT_EQ(assigned_map_val, map_val);

  std::map<std::string, Value> modified_map = *map_val.AsStructure();
  modified_map["key"] = Value(kTestVal6);
  map_val = Value(modified_map);

  EXPECT_NE(assigned_map_val, map_val);
  EXPECT_EQ(assigned_map_val.AsStructure()->at("key").AsInt(), kExpectedInt5);
  EXPECT_EQ(map_val.AsStructure()->at("key").AsInt(), kExpectedInt64_6);
}

TEST(ValueTest, IsNumberHandlesIntAndDouble) {
  Value int_val(kTestVal100);
  EXPECT_TRUE(int_val.IsNumber());

  Value double_val(kTestDoublePi);
  EXPECT_TRUE(double_val.IsNumber());

  Value bool_val(true);
  EXPECT_FALSE(bool_val.IsNumber());

  Value string_val("hello");
  EXPECT_FALSE(string_val.IsNumber());
}

TEST(ValueTest, AsNumberConversions) {
  Value int_val(kTestVal5);
  EXPECT_EQ(int_val.AsInt(), kExpectedInt5);
  EXPECT_EQ(int_val.AsDouble(), kTestDouble50);

  Value double_val(kTestDouble57);
  EXPECT_EQ(double_val.AsInt(), kExpectedRoundedUp6);
  EXPECT_EQ(double_val.AsDouble(), kTestDouble57);

  Value double_val_negative(kNegative53);
  EXPECT_EQ(double_val_negative.AsInt(),
            kExpectedNegative5);  // Rounds to nearest even on .5, otherwise
                                  // standard rounding
  EXPECT_EQ(double_val_negative.AsDouble(), kNegative53);

  Value double_val_negative_half(kNegative55);
  EXPECT_EQ(double_val_negative_half.AsInt(),
            kExpectedNegative5);  // Rounds to nearest even
}

TEST(ValueTest, EqualityOperator_BasicTypes) {
  EXPECT_TRUE(Value(true) == Value(true));
  EXPECT_FALSE(Value(true) == Value(false));
  EXPECT_TRUE(Value(kTestVal10) == Value(kTestVal10));
  EXPECT_FALSE(Value(kTestVal10) == Value(kTestVal11));
  EXPECT_TRUE(Value(kTestDoublePi) == Value(kTestDoublePi));
  EXPECT_FALSE(Value(kTestDoublePi) == Value(kTestDoublePiOther));
  EXPECT_TRUE(Value("test") == Value("test"));
  EXPECT_FALSE(Value("test") == Value("other"));
  EXPECT_TRUE(Value() == Value());

  auto time1 = std::chrono::system_clock::now();
  auto time2 = time1 + 1s;
  EXPECT_TRUE(Value(time1) == Value(time1));
  EXPECT_FALSE(Value(time1) == Value(time2));
}

TEST(ValueTest, EqualityOperator_NumberCrossTypes) {
  EXPECT_TRUE(Value(kTestVal5) == Value(kTestDouble50));
  EXPECT_TRUE(Value(kTestDouble50) == Value(kTestVal5));
  EXPECT_FALSE(Value(kTestVal5) == Value(kTestDouble51));
  EXPECT_FALSE(Value(kTestDouble51) == Value(kTestVal5));
  EXPECT_TRUE(Value(std::numeric_limits<int64_t>::max()) ==
              Value(static_cast<double>(std::numeric_limits<int64_t>::max())));
}

TEST(ValueTest, EqualityOperator_DifferentTypes) {
  EXPECT_FALSE(Value(true) == Value(1));
  EXPECT_FALSE(Value(kTestVal10) == Value("10"));
  EXPECT_FALSE(Value(kTestDoublePi) == Value(true));
  EXPECT_FALSE(Value("hello") == Value());
  EXPECT_FALSE(Value() == Value(false));
}

TEST(ValueTest, EqualityOperator_ComplexTypes) {
  std::map<std::string, Value> map1 = {{"a", Value(1)}, {"b", Value("x")}};
  std::map<std::string, Value> map2 = {{"a", Value(1)}, {"b", Value("x")}};
  std::map<std::string, Value> map3 = {{"a", Value(1)}, {"b", Value("y")}};
  std::map<std::string, Value> map4 = {{"a", Value(1)}};

  EXPECT_TRUE(Value(map1) == Value(map2));
  EXPECT_FALSE(Value(map1) == Value(map3));
  EXPECT_FALSE(Value(map1) == Value(map4));
  EXPECT_FALSE(Value(map1) == Value());

  std::vector<Value> list1 = {Value(1), Value("x")};
  std::vector<Value> list2 = {Value(1), Value("x")};
  std::vector<Value> list3 = {Value(1), Value("y")};
  std::vector<Value> list4 = {Value(1)};

  EXPECT_TRUE(Value(list1) == Value(list2));
  EXPECT_FALSE(Value(list1) == Value(list3));
  EXPECT_FALSE(Value(list1) == Value(list4));
  EXPECT_FALSE(Value(list1) == Value());

  std::map<std::string, Value> nested_map1 = {{"key", Value(list1)}};
  std::map<std::string, Value> nested_map2 = {{"key", Value(list2)}};
  std::map<std::string, Value> nested_map3 = {{"key", Value(list3)}};

  EXPECT_TRUE(Value(nested_map1) == Value(nested_map2));
  EXPECT_FALSE(Value(nested_map1) == Value(nested_map3));
}

TEST(ValueTest, InequalityOperator) {
  EXPECT_TRUE(Value(true) != Value(false));
  EXPECT_TRUE(Value(kTestVal10) != Value(kTestVal11));
  EXPECT_TRUE(Value("test") != Value("other"));
  EXPECT_TRUE(Value(kTestVal5) != Value(kTestDouble51));
  EXPECT_TRUE(Value(true) != Value(1));
  EXPECT_TRUE(Value("hello") != Value());

  std::map<std::string, Value> map1 = {{"a", Value(1)}};
  std::map<std::string, Value> map2 = {{"a", Value(2)}};
  EXPECT_TRUE(Value(map1) != Value(map2));

  std::vector<Value> list1 = {Value(1)};
  std::vector<Value> list2 = {Value(2)};
  EXPECT_TRUE(Value(list1) != Value(list2));

  EXPECT_FALSE(Value(true) != Value(true));
  EXPECT_FALSE(Value() != Value());
  EXPECT_FALSE(Value(kTestVal5) != Value(kTestDouble50));
}

TEST(ValueTest, MoveConstructorAndAssignmentDefaulted) {
  // Move constructor
  Value original_string("move me");
  Value moved_string = std::move(original_string);
  EXPECT_TRUE(moved_string.IsString());
  EXPECT_EQ(moved_string.AsString(), "move me");

  // Move assignment
  Value original_map_val;
  std::map<std::string, Value> data_map = {{"key", Value(kTestVal100)}};
  original_map_val = Value(data_map);

  Value target_val("old value");
  target_val = std::move(original_map_val);

  EXPECT_TRUE(target_val.IsStructure());
  EXPECT_NE(target_val.AsStructure(), nullptr);
  EXPECT_EQ(target_val.AsStructure()->at("key").AsInt(), kExpectedInt100);
}

TEST(ValueTest, ToStringAndStreamOperator) {
  // Null
  EXPECT_EQ(Value().ToString(), "null");
  // Boolean
  EXPECT_EQ(Value(true).ToString(), "true");
  EXPECT_EQ(Value(false).ToString(), "false");
  // Numbers
  EXPECT_EQ(Value(static_cast<int>(kTestInt64)).ToString(), "123");
  EXPECT_EQ(Value(kExpectedInt64).ToString(), "456");
  // String
  EXPECT_EQ(Value("hello").ToString(), "\"hello\"");
  // List
  std::vector<Value> list = {Value("item1"), Value(kListItem42), Value(true)};
  EXPECT_EQ(Value(list).ToString(), "[\"item1\", 42, true]");
  // Structure / Map
  std::map<std::string, Value> map = {{"key", Value("value")}};
  EXPECT_EQ(Value(map).ToString(), "{\"key\": \"value\"}");
  // Stream operator <<
  std::ostringstream stream;
  stream << Value("stream_test");
  EXPECT_EQ(stream.str(), "\"stream_test\"");
}

}  // namespace openfeature
