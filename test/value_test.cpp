#include "openfeature/value.h"

#include <chrono>
#include <limits>
#include <map>
#include <vector>

#include "gtest/gtest.h"

namespace openfeature {

using namespace std::chrono_literals;

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
  Value int_val(static_cast<int64_t>(123LL));
  EXPECT_FALSE(int_val.IsNull());
  EXPECT_TRUE(int_val.IsNumber());
  EXPECT_EQ(int_val.AsInt(), 123LL);
  EXPECT_EQ(int_val.AsDouble(), 123.0);
  EXPECT_FALSE(int_val.AsBool().has_value());

  Value int_min(std::numeric_limits<int64_t>::min());
  EXPECT_EQ(int_min.AsInt(), std::numeric_limits<int64_t>::min());

  Value int_max(std::numeric_limits<int64_t>::max());
  EXPECT_EQ(int_max.AsInt(), std::numeric_limits<int64_t>::max());
}

TEST(ValueTest, IntConstructorAndAccessors) {
  Value int_val(456);
  EXPECT_FALSE(int_val.IsNull());
  EXPECT_TRUE(int_val.IsNumber());
  EXPECT_EQ(int_val.AsInt(), 456LL);
  EXPECT_EQ(int_val.AsDouble(), 456.0);
}

TEST(ValueTest, DoubleConstructorAndAccessors) {
  Value double_val(123.45);
  EXPECT_FALSE(double_val.IsNull());
  EXPECT_TRUE(double_val.IsNumber());
  EXPECT_EQ(double_val.AsDouble(), 123.45);
  EXPECT_EQ(double_val.AsInt(), 123LL);  // Should round
  EXPECT_FALSE(double_val.AsBool().has_value());

  Value double_round_up(123.5);
  EXPECT_EQ(double_round_up.AsInt(), 124LL);

  Value double_round_down(123.4);
  EXPECT_EQ(double_round_down.AsInt(), 123LL);
}

TEST(ValueTest, StringConstructorAndAccessors) {
  Value string_val(std::string("hello"));
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
  test_map["key3"] = Value(123);

  Value map_val(test_map);
  EXPECT_FALSE(map_val.IsNull());
  EXPECT_TRUE(map_val.IsStructure());
  EXPECT_NE(map_val.AsStructure(), nullptr);
  ASSERT_TRUE(map_val.AsStructure()->count("key1"));
  EXPECT_EQ(map_val.AsStructure()->at("key1").AsString(), "value1");
  EXPECT_EQ(map_val.AsStructure()->at("key2").AsBool(), true);
  EXPECT_EQ(map_val.AsStructure()->at("key3").AsInt(), 123LL);

  EXPECT_FALSE(map_val.AsBool().has_value());
  EXPECT_EQ(map_val.AsList(), nullptr);

  Value empty_map_val(std::map<std::string, Value>{});
  EXPECT_TRUE(empty_map_val.IsStructure());
  EXPECT_NE(empty_map_val.AsStructure(), nullptr);
  EXPECT_TRUE(empty_map_val.AsStructure()->empty());
}

TEST(ValueTest, VectorConstructorAndAccessors) {
  std::vector<Value> test_list;
  test_list.push_back(Value("item1"));
  test_list.push_back(Value(false));
  test_list.push_back(Value(4.5));

  Value list_val(test_list);
  EXPECT_FALSE(list_val.IsNull());
  EXPECT_TRUE(list_val.IsList());
  EXPECT_NE(list_val.AsList(), nullptr);
  ASSERT_EQ(list_val.AsList()->size(), 3);
  EXPECT_EQ(list_val.AsList()->at(0).AsString(), "item1");
  EXPECT_EQ(list_val.AsList()->at(1).AsBool(), false);
  EXPECT_EQ(list_val.AsList()->at(2).AsDouble(), 4.5);

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
  inner_map["nested_key"] = Value(100);
  std::vector<Value> inner_list = {Value("list_item")};

  std::map<std::string, Value> original_map;
  original_map["bool"] = Value(true);
  original_map["int"] = Value(123);
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
            100LL);
  EXPECT_EQ(copied_value.AsStructure()
                ->at("sub_map")
                .AsStructure()
                ->at("nested_key")
                .AsInt(),
            100LL);

  std::map<std::string, Value> original_nested_map =
      *original_value.AsStructure()->at("sub_map").AsStructure();
  original_nested_map["nested_key"] = Value(200);
  original_map["sub_map"] = Value(original_nested_map);
  original_value = Value(original_map);

  EXPECT_EQ(original_value.AsStructure()
                ->at("sub_map")
                .AsStructure()
                ->at("nested_key")
                .AsInt(),
            200LL);
  EXPECT_EQ(copied_value.AsStructure()
                ->at("sub_map")
                .AsStructure()
                ->at("nested_key")
                .AsInt(),
            100LL);
}

TEST(ValueTest, AssignmentOperator) {
  Value val1(10);
  Value val2("test");
  Value val3;

  val3 = val1;
  EXPECT_EQ(val3.AsInt(), 10LL);
  EXPECT_EQ(val3, val1);

  val3 = val2;
  EXPECT_EQ(val3.AsString(), "test");
  EXPECT_EQ(val3, val2);

  std::map<std::string, Value> original_map;
  original_map["key"] = Value(5);
  Value map_val(original_map);

  Value assigned_map_val;
  assigned_map_val = map_val;
  EXPECT_EQ(assigned_map_val, map_val);

  std::map<std::string, Value> modified_map = *map_val.AsStructure();
  modified_map["key"] = Value(6);
  map_val = Value(modified_map);

  EXPECT_NE(assigned_map_val, map_val);
  EXPECT_EQ(assigned_map_val.AsStructure()->at("key").AsInt(), 5LL);
  EXPECT_EQ(map_val.AsStructure()->at("key").AsInt(), 6LL);
}

TEST(ValueTest, IsNumberHandlesIntAndDouble) {
  Value int_val(100);
  EXPECT_TRUE(int_val.IsNumber());

  Value double_val(3.14);
  EXPECT_TRUE(double_val.IsNumber());

  Value bool_val(true);
  EXPECT_FALSE(bool_val.IsNumber());

  Value string_val("hello");
  EXPECT_FALSE(string_val.IsNumber());
}

TEST(ValueTest, AsNumberConversions) {
  Value int_val(5);
  EXPECT_EQ(int_val.AsInt(), 5LL);
  EXPECT_EQ(int_val.AsDouble(), 5.0);

  Value double_val(5.7);
  EXPECT_EQ(double_val.AsInt(), 6LL);
  EXPECT_EQ(double_val.AsDouble(), 5.7);

  Value double_val_negative(-5.3);
  EXPECT_EQ(double_val_negative.AsInt(),
            -5LL);  // Rounds to nearest even on .5, otherwise standard rounding
  EXPECT_EQ(double_val_negative.AsDouble(), -5.3);

  Value double_val_negative_half(-5.5);
  EXPECT_EQ(double_val_negative_half.AsInt(), -5LL);  // Rounds to nearest even
}

TEST(ValueTest, EqualityOperator_BasicTypes) {
  EXPECT_TRUE(Value(true) == Value(true));
  EXPECT_FALSE(Value(true) == Value(false));
  EXPECT_TRUE(Value(10) == Value(10));
  EXPECT_FALSE(Value(10) == Value(11));
  EXPECT_TRUE(Value(3.14) == Value(3.14));
  EXPECT_FALSE(Value(3.14) == Value(3.15));
  EXPECT_TRUE(Value("test") == Value("test"));
  EXPECT_FALSE(Value("test") == Value("other"));
  EXPECT_TRUE(Value() == Value());

  auto t1 = std::chrono::system_clock::now();
  auto t2 = t1 + 1s;
  EXPECT_TRUE(Value(t1) == Value(t1));
  EXPECT_FALSE(Value(t1) == Value(t2));
}

TEST(ValueTest, EqualityOperator_NumberCrossTypes) {
  EXPECT_TRUE(Value(5) == Value(5.0));
  EXPECT_TRUE(Value(5.0) == Value(5));
  EXPECT_FALSE(Value(5) == Value(5.1));
  EXPECT_FALSE(Value(5.1) == Value(5));
  EXPECT_TRUE(Value(std::numeric_limits<int64_t>::max()) ==
              Value(static_cast<double>(std::numeric_limits<int64_t>::max())));
}

TEST(ValueTest, EqualityOperator_DifferentTypes) {
  EXPECT_FALSE(Value(true) == Value(1));
  EXPECT_FALSE(Value(10) == Value("10"));
  EXPECT_FALSE(Value(3.14) == Value(true));
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
  EXPECT_TRUE(Value(10) != Value(11));
  EXPECT_TRUE(Value("test") != Value("other"));
  EXPECT_TRUE(Value(5) != Value(5.1));
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
  EXPECT_FALSE(Value(5) != Value(5.0));
}

TEST(ValueTest, MoveConstructorAndAssignmentDefaulted) {
  // Move constructor
  Value original_string("move me");
  Value moved_string = std::move(original_string);
  EXPECT_TRUE(moved_string.IsString());
  EXPECT_EQ(moved_string.AsString(), "move me");

  // Move assignment
  Value original_map_val;
  std::map<std::string, Value> data_map = {{"key", Value(100)}};
  original_map_val = Value(data_map);

  Value target_val("old value");
  target_val = std::move(original_map_val);

  EXPECT_TRUE(target_val.IsStructure());
  EXPECT_NE(target_val.AsStructure(), nullptr);
  EXPECT_EQ(target_val.AsStructure()->at("key").AsInt(), 100LL);
}

}  // namespace openfeature