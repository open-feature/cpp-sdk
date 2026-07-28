#include "openfeature/hook_context.h"

#include <gtest/gtest.h>

#include <map>
#include <memory>
#include <string>

#include "openfeature/evaluation_context.h"
#include "openfeature/flag_metadata.h"
#include "openfeature/flag_type_value.h"
#include "openfeature/hook_data.h"
#include "openfeature/metadata.h"
#include "openfeature/value.h"

namespace openfeature {

class HookContextTest : public ::testing::Test {
 protected:
  HookContextTest()
      : initial_ctx_(EvaluationContext::Builder()
                         .WithTargetingKey("initial-user")
                         .WithAttribute("env", std::string("test"))
                         .build()) {}

  EvaluationContext initial_ctx_;
  Metadata client_metadata_{"test-client"};
  Metadata provider_metadata_{"test-provider"};
  std::shared_ptr<HookData> hook_data_{std::make_shared<HookData>()};
};

TEST_F(HookContextTest, ConstructorAndAccessorsForBool) {
  constexpr FlagValueType kType = FlagValueType::kBoolean;
  constexpr bool kDefaultValue = true;

  BoolHookContext hook_ctx("bool-flag", kType, kDefaultValue, initial_ctx_,
                           client_metadata_, provider_metadata_, hook_data_);

  EXPECT_EQ(hook_ctx.GetFlagKey(), "bool-flag");
  EXPECT_EQ(hook_ctx.GetType(), kType);
  EXPECT_EQ(hook_ctx.GetDefaultValue(), kDefaultValue);

  ASSERT_TRUE(hook_ctx.GetEvaluationContext().GetTargetingKey().has_value());
  EXPECT_EQ(hook_ctx.GetEvaluationContext().GetTargetingKey().value(),
            "initial-user");

  EXPECT_EQ(hook_ctx.GetClientMetadata().name, "test-client");
  EXPECT_EQ(hook_ctx.GetProviderMetadata().name, "test-provider");
  EXPECT_EQ(hook_ctx.GetHookData(), hook_data_);

  Value default_val = hook_ctx.GetDefaultValueAsValue();
  ASSERT_TRUE(default_val.IsBool());
  EXPECT_EQ(default_val.AsBool().value(), kDefaultValue);

  // Upcast to general interface reference (as a General Hook receives):
  GeneralHookContext& general_ctx = hook_ctx;

  EXPECT_EQ(general_ctx.GetFlagKey(), "bool-flag");
  EXPECT_EQ(general_ctx.GetType(), kType);
  // EXPECT_EQ(general_ctx.GetDefaultValue(), kDefaultValue);

  ASSERT_TRUE(general_ctx.GetEvaluationContext().GetTargetingKey().has_value());
  EXPECT_EQ(general_ctx.GetEvaluationContext().GetTargetingKey().value(),
            "initial-user");

  EXPECT_EQ(general_ctx.GetClientMetadata().name, "test-client");
  EXPECT_EQ(general_ctx.GetProviderMetadata().name, "test-provider");
  EXPECT_EQ(general_ctx.GetHookData(), hook_data_);

  default_val = general_ctx.GetDefaultValueAsValue();
  ASSERT_TRUE(default_val.IsBool());
  EXPECT_EQ(default_val.AsBool().value(), kDefaultValue);
}

TEST_F(HookContextTest, ConstructorAndAccessorsForString) {
  constexpr FlagValueType kType = FlagValueType::kString;
  const std::string default_value = "default-string";

  StringHookContext hook_ctx("string-flag", kType, default_value, initial_ctx_,
                             client_metadata_, provider_metadata_, hook_data_);

  EXPECT_EQ(hook_ctx.GetFlagKey(), "string-flag");
  EXPECT_EQ(hook_ctx.GetType(), kType);
  EXPECT_EQ(hook_ctx.GetDefaultValue(), default_value);

  ASSERT_TRUE(hook_ctx.GetEvaluationContext().GetTargetingKey().has_value());
  EXPECT_EQ(hook_ctx.GetEvaluationContext().GetTargetingKey().value(),
            "initial-user");

  EXPECT_EQ(hook_ctx.GetClientMetadata().name, "test-client");
  EXPECT_EQ(hook_ctx.GetProviderMetadata().name, "test-provider");
  EXPECT_EQ(hook_ctx.GetHookData(), hook_data_);

  Value default_val = hook_ctx.GetDefaultValueAsValue();
  ASSERT_TRUE(default_val.IsString());
  EXPECT_EQ(default_val.AsString().value(), default_value);

  // Upcast to general interface reference (as a General Hook receives):
  GeneralHookContext& general_ctx = hook_ctx;

  EXPECT_EQ(general_ctx.GetFlagKey(), "string-flag");
  EXPECT_EQ(general_ctx.GetType(), kType);

  ASSERT_TRUE(general_ctx.GetEvaluationContext().GetTargetingKey().has_value());
  EXPECT_EQ(general_ctx.GetEvaluationContext().GetTargetingKey().value(),
            "initial-user");

  EXPECT_EQ(general_ctx.GetClientMetadata().name, "test-client");
  EXPECT_EQ(general_ctx.GetProviderMetadata().name, "test-provider");
  EXPECT_EQ(general_ctx.GetHookData(), hook_data_);

  default_val = general_ctx.GetDefaultValueAsValue();
  ASSERT_TRUE(default_val.IsString());
  EXPECT_EQ(default_val.AsString().value(), default_value);
}

TEST_F(HookContextTest, ConstructorAndAccessorsForInteger) {
  constexpr FlagValueType kType = FlagValueType::kInteger;
  constexpr int64_t kDefaultValue = 123456789LL;

  IntHookContext hook_ctx("int-flag", kType, kDefaultValue, initial_ctx_,
                          client_metadata_, provider_metadata_, hook_data_);

  EXPECT_EQ(hook_ctx.GetFlagKey(), "int-flag");
  EXPECT_EQ(hook_ctx.GetType(), kType);
  EXPECT_EQ(hook_ctx.GetDefaultValue(), kDefaultValue);

  ASSERT_TRUE(hook_ctx.GetEvaluationContext().GetTargetingKey().has_value());
  EXPECT_EQ(hook_ctx.GetEvaluationContext().GetTargetingKey().value(),
            "initial-user");

  EXPECT_EQ(hook_ctx.GetClientMetadata().name, "test-client");
  EXPECT_EQ(hook_ctx.GetProviderMetadata().name, "test-provider");
  EXPECT_EQ(hook_ctx.GetHookData(), hook_data_);

  Value default_val = hook_ctx.GetDefaultValueAsValue();
  ASSERT_TRUE(default_val.IsNumber());
  EXPECT_EQ(default_val.AsInt().value(), kDefaultValue);

  // Upcast to general interface reference (as a General Hook receives):
  GeneralHookContext& general_ctx = hook_ctx;

  EXPECT_EQ(general_ctx.GetFlagKey(), "int-flag");
  EXPECT_EQ(general_ctx.GetType(), kType);

  ASSERT_TRUE(general_ctx.GetEvaluationContext().GetTargetingKey().has_value());
  EXPECT_EQ(general_ctx.GetEvaluationContext().GetTargetingKey().value(),
            "initial-user");

  EXPECT_EQ(general_ctx.GetClientMetadata().name, "test-client");
  EXPECT_EQ(general_ctx.GetProviderMetadata().name, "test-provider");
  EXPECT_EQ(general_ctx.GetHookData(), hook_data_);

  default_val = general_ctx.GetDefaultValueAsValue();
  ASSERT_TRUE(default_val.IsNumber());
  EXPECT_EQ(default_val.AsInt().value(), kDefaultValue);
}

TEST_F(HookContextTest, ConstructorAndAccessorsForDouble) {
  constexpr FlagValueType kType = FlagValueType::kDouble;
  constexpr double kDefaultValue = 3.14159265359;

  DoubleHookContext hook_ctx("double-flag", kType, kDefaultValue, initial_ctx_,
                             client_metadata_, provider_metadata_, hook_data_);

  EXPECT_EQ(hook_ctx.GetFlagKey(), "double-flag");
  EXPECT_EQ(hook_ctx.GetType(), kType);
  EXPECT_DOUBLE_EQ(hook_ctx.GetDefaultValue(), kDefaultValue);

  ASSERT_TRUE(hook_ctx.GetEvaluationContext().GetTargetingKey().has_value());
  EXPECT_EQ(hook_ctx.GetEvaluationContext().GetTargetingKey().value(),
            "initial-user");

  EXPECT_EQ(hook_ctx.GetClientMetadata().name, "test-client");
  EXPECT_EQ(hook_ctx.GetProviderMetadata().name, "test-provider");
  EXPECT_EQ(hook_ctx.GetHookData(), hook_data_);

  Value default_val = hook_ctx.GetDefaultValueAsValue();
  ASSERT_TRUE(default_val.IsNumber());
  EXPECT_DOUBLE_EQ(default_val.AsDouble().value(), kDefaultValue);

  // Upcast to general interface reference (as a General Hook receives):
  GeneralHookContext& general_ctx = hook_ctx;

  EXPECT_EQ(general_ctx.GetFlagKey(), "double-flag");
  EXPECT_EQ(general_ctx.GetType(), kType);

  ASSERT_TRUE(general_ctx.GetEvaluationContext().GetTargetingKey().has_value());
  EXPECT_EQ(general_ctx.GetEvaluationContext().GetTargetingKey().value(),
            "initial-user");

  EXPECT_EQ(general_ctx.GetClientMetadata().name, "test-client");
  EXPECT_EQ(general_ctx.GetProviderMetadata().name, "test-provider");
  EXPECT_EQ(general_ctx.GetHookData(), hook_data_);

  default_val = general_ctx.GetDefaultValueAsValue();
  ASSERT_TRUE(default_val.IsNumber());
  EXPECT_DOUBLE_EQ(default_val.AsDouble().value(), kDefaultValue);
}

TEST_F(HookContextTest, ConstructorAndAccessorsForObject) {
  constexpr FlagValueType kType = FlagValueType::kObject;
  std::map<std::string, Value> object_map{
      {"feature_enabled", Value(true)},
      {"max_items", Value(int64_t(100LL))},
      {"theme", Value(std::string("dark"))}};
  Value default_value(object_map);

  ObjectHookContext hook_ctx("object-flag", kType, default_value, initial_ctx_,
                             client_metadata_, provider_metadata_, hook_data_);

  EXPECT_EQ(hook_ctx.GetFlagKey(), "object-flag");
  EXPECT_EQ(hook_ctx.GetType(), kType);
  ASSERT_TRUE(hook_ctx.GetDefaultValue().IsStructure());
  const auto* struct_map = hook_ctx.GetDefaultValue().AsStructure();
  ASSERT_NE(struct_map, nullptr);
  EXPECT_EQ(struct_map->at("feature_enabled").AsBool().value(), true);
  EXPECT_EQ(struct_map->at("max_items").AsInt().value(), 100LL);
  EXPECT_EQ(struct_map->at("theme").AsString().value(), "dark");

  ASSERT_TRUE(hook_ctx.GetEvaluationContext().GetTargetingKey().has_value());
  EXPECT_EQ(hook_ctx.GetEvaluationContext().GetTargetingKey().value(),
            "initial-user");

  EXPECT_EQ(hook_ctx.GetClientMetadata().name, "test-client");
  EXPECT_EQ(hook_ctx.GetProviderMetadata().name, "test-provider");
  EXPECT_EQ(hook_ctx.GetHookData(), hook_data_);

  Value default_val = hook_ctx.GetDefaultValueAsValue();
  ASSERT_TRUE(default_val.IsStructure());
  ASSERT_NE(default_val.AsStructure(), nullptr);
  EXPECT_EQ(default_val.AsStructure()->at("feature_enabled").AsBool().value(),
            true);
  EXPECT_EQ(default_val.AsStructure()->at("max_items").AsInt().value(), 100LL);
  EXPECT_EQ(default_val.AsStructure()->at("theme").AsString().value(), "dark");

  // Upcast to general interface reference (as a General Hook receives):
  GeneralHookContext& general_ctx = hook_ctx;

  EXPECT_EQ(general_ctx.GetFlagKey(), "object-flag");
  EXPECT_EQ(general_ctx.GetType(), kType);

  ASSERT_TRUE(general_ctx.GetEvaluationContext().GetTargetingKey().has_value());
  EXPECT_EQ(general_ctx.GetEvaluationContext().GetTargetingKey().value(),
            "initial-user");

  EXPECT_EQ(general_ctx.GetClientMetadata().name, "test-client");
  EXPECT_EQ(general_ctx.GetProviderMetadata().name, "test-provider");
  EXPECT_EQ(general_ctx.GetHookData(), hook_data_);

  default_val = general_ctx.GetDefaultValueAsValue();
  ASSERT_TRUE(default_val.IsStructure());
  ASSERT_NE(default_val.AsStructure(), nullptr);
  EXPECT_EQ(default_val.AsStructure()->at("feature_enabled").AsBool().value(),
            true);
  EXPECT_EQ(default_val.AsStructure()->at("max_items").AsInt().value(), 100LL);
  EXPECT_EQ(default_val.AsStructure()->at("theme").AsString().value(), "dark");
}

TEST_F(HookContextTest, SetEvaluationContextUpdatesContext) {
  BoolHookContext hook_ctx("flag-key", FlagValueType::kBoolean, false,
                           initial_ctx_, client_metadata_, provider_metadata_,
                           hook_data_);

  ASSERT_TRUE(hook_ctx.GetEvaluationContext().GetTargetingKey().has_value());
  EXPECT_EQ(hook_ctx.GetEvaluationContext().GetTargetingKey().value(),
            "initial-user");

  EvaluationContext updated_ctx = EvaluationContext::Builder()
                                      .WithTargetingKey("updated-user")
                                      .WithAttribute("env", std::string("prod"))
                                      .build();

  hook_ctx.SetEvaluationContext(std::move(updated_ctx));

  ASSERT_TRUE(hook_ctx.GetEvaluationContext().GetTargetingKey().has_value());
  EXPECT_EQ(hook_ctx.GetEvaluationContext().GetTargetingKey().value(),
            "updated-user");

  const std::any* env_val = hook_ctx.GetEvaluationContext().GetValue("env");
  ASSERT_NE(env_val, nullptr);
  EXPECT_EQ(std::any_cast<std::string>(*env_val), "prod");
}

TEST_F(HookContextTest, HookDataSharesStateAndAllowsMutations) {
  BoolHookContext hook_ctx("flag-key", FlagValueType::kBoolean, true,
                           initial_ctx_, client_metadata_, provider_metadata_,
                           hook_data_);

  ASSERT_NE(hook_ctx.GetHookData(), nullptr);
  EXPECT_EQ(hook_ctx.GetHookData()->Get("custom_key"), nullptr);

  hook_ctx.GetHookData()->Set("custom_key", std::string("stage_before"));

  auto* retrieved = hook_ctx.GetHookData()->GetAs<std::string>("custom_key");
  ASSERT_NE(retrieved, nullptr);
  EXPECT_EQ(*retrieved, "stage_before");
}

TEST_F(HookContextTest, HandlesNullptrHookData) {
  BoolHookContext hook_ctx("flag-key", FlagValueType::kBoolean, false,
                           initial_ctx_, client_metadata_, provider_metadata_,
                           nullptr);

  EXPECT_EQ(hook_ctx.GetHookData(), nullptr);
}

}  // namespace openfeature
