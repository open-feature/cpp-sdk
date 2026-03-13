#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <any>
#include <chrono>
#include <cucumber.hpp>
#include <map>
#include <memory>
#include <string>
#include <thread>
#include <unordered_map>

#include "openfeature/evaluation_context.h"
#include "openfeature/memory_provider/flag.h"
#include "openfeature/memory_provider/in_memory_provider.h"
#include "openfeature/openfeature_api.h"
#include "test/e2e/context_storing_provider.h"
#include "test/e2e/state.h"
#include "test/mocks/mock_feature_provider.h"

using ::testing::_;
using ::testing::Return;

constexpr int64_t kDefaultIntegerFlagValue = 10;
constexpr double kDefaultFloatFlagValue = 0.5;
constexpr int64_t kImagesPerPageValue = 100;
constexpr auto kProviderInitDelay = std::chrono::milliseconds(200);

// Helper to create simple static flags for the InMemoryProvider.
template <typename T>
openfeature::Flag<T> CreateStaticFlag(T value) {
  return openfeature::Flag<T>(
      {{"default", value}}, "default",
      [value](const openfeature::Flag<T>&,
              const openfeature::EvaluationContext&) { return value; },
      openfeature::FlagMetadata{});
}

std::shared_ptr<openfeature::FeatureProvider> CreateStableProvider() {
  std::unordered_map<std::string, std::any> flags;

  // Set up the static flags expected by the basic evaluation tests.
  flags["boolean-flag"] = CreateStaticFlag<bool>(true);
  flags["string-flag"] = CreateStaticFlag<std::string>("hi");
  flags["integer-flag"] = CreateStaticFlag<int64_t>(kDefaultIntegerFlagValue);
  flags["float-flag"] = CreateStaticFlag<double>(kDefaultFloatFlagValue);

  // Object flag setup.
  std::map<std::string, openfeature::Value> obj_map;
  obj_map["showImages"] = openfeature::Value(true);
  obj_map["title"] = openfeature::Value("Check out these pics!");
  obj_map["imagesPerPage"] = openfeature::Value(kImagesPerPageValue);
  flags["object-flag"] =
      CreateStaticFlag<openfeature::Value>(openfeature::Value(obj_map));

  // Context-aware flag setup.
  auto context_evaluator = [](const openfeature::Flag<std::string>&,
                              const openfeature::EvaluationContext& ctx)
      -> absl::StatusOr<std::string> {
    const std::any* customer = ctx.GetValue("customer");
    if (customer && std::any_cast<std::string>(*customer) == "false") {
      return "INTERNAL";
    }
    return "EXTERNAL";
  };

  flags["context-aware"] = openfeature::Flag<std::string>(
      {{"internal", "INTERNAL"}, {"external", "EXTERNAL"}}, "external",
      context_evaluator, openfeature::FlagMetadata{});

  return std::make_shared<openfeature::InMemoryProvider>(std::move(flags));
}

std::shared_ptr<openfeature::FeatureProvider> CreateMockErrorProvider() {
  std::shared_ptr<openfeature::MockFeatureProvider> mock =
      std::make_shared<openfeature::MockFeatureProvider>();
  EXPECT_CALL(*mock, GetMetadata())
      .WillRepeatedly(Return(openfeature::Metadata{"MockFeatureProvider"}));
  EXPECT_CALL(*mock, Shutdown()).WillRepeatedly(Return(absl::OkStatus()));
  EXPECT_CALL(*mock, Init(_))
      .WillOnce(Return(absl::UnknownError("Simulated Error")));
  return mock;
}

std::shared_ptr<openfeature::FeatureProvider> CreateMockNotReadyProvider() {
  std::shared_ptr<openfeature::MockFeatureProvider> mock =
      std::make_shared<openfeature::MockFeatureProvider>();
  EXPECT_CALL(*mock, GetMetadata())
      .WillRepeatedly(Return(openfeature::Metadata{"MockFeatureProvider"}));
  EXPECT_CALL(*mock, Shutdown()).WillRepeatedly(Return(absl::OkStatus()));
  EXPECT_CALL(*mock, Init(_))
      .WillOnce(testing::Invoke([](const openfeature::EvaluationContext&) {
        std::this_thread::sleep_for(kProviderInitDelay);
        return absl::OkStatus();
      }));
  return mock;
}

GIVEN(provider_setup, "a {word} provider") {
  std::string status_type = CUKE_ARG(1);
  auto& state = cuke::context<openfeature_e2e::State>();

  if (status_type == "stable" || status_type == "ready") {
    state.provider = CreateStableProvider();
    openfeature::OpenFeatureAPI::GetInstance().SetProviderAndWait(
        state.provider);
  } else if (status_type == "error") {
    state.provider = CreateMockErrorProvider();
    openfeature::OpenFeatureAPI::GetInstance().SetProviderAndWait(
        state.provider);
  } else if (status_type == "not ready") {
    state.provider = CreateMockNotReadyProvider();
    openfeature::OpenFeatureAPI::GetInstance().SetProvider(state.provider);
  }

  state.client = openfeature::OpenFeatureAPI::GetInstance().GetClient();
}

THEN(check_provider_status, "the provider status should be {string}") {
  std::string expected_status_str = CUKE_ARG(1);
  auto& state = cuke::context<openfeature_e2e::State>();

  openfeature::ProviderStatus actual_status = state.client->GetProviderStatus();
  openfeature::ProviderStatus expected_status =
      openfeature::ProviderStatus::kReady;

  if (expected_status_str == "READY") {
    expected_status = openfeature::ProviderStatus::kReady;
  } else if (expected_status_str == "NOT_READY") {
    expected_status = openfeature::ProviderStatus::kNotReady;
  } else if (expected_status_str == "ERROR") {
    expected_status = openfeature::ProviderStatus::kError;
  } else if (expected_status_str == "FATAL") {
    expected_status = openfeature::ProviderStatus::kFatal;
  } else if (expected_status_str == "STALE") {
    expected_status = openfeature::ProviderStatus::kStale;
  }

  EXPECT_EQ(actual_status, expected_status);
}

WHEN(eval_boolean_flag,
     "a boolean flag with key {string} is evaluated with default value "
     "{string}") {
  std::string key = CUKE_ARG(1);
  std::string default_val_str = CUKE_ARG(2);
  auto& state = cuke::context<openfeature_e2e::State>();

  bool default_val = (default_val_str == "true");
  state.last_evaluation_value =
      openfeature::Value(state.client->GetBooleanValue(key, default_val));
}

THEN(check_resolved_boolean, "the resolved boolean value should be {string}") {
  std::string expected_str = CUKE_ARG(1);
  auto& state = cuke::context<openfeature_e2e::State>();

  bool expected = (expected_str == "true");
  EXPECT_EQ(state.last_evaluation_value.AsBool().value(), expected);
}

WHEN(eval_string_flag,
     "a string flag with key {string} is evaluated with default value "
     "{string}") {
  std::string key = CUKE_ARG(1);
  std::string default_val = CUKE_ARG(2);
  auto& state = cuke::context<openfeature_e2e::State>();

  state.last_evaluation_value =
      openfeature::Value(state.client->GetStringValue(key, default_val));
}

THEN(check_resolved_string, "the resolved string value should be {string}") {
  std::string expected = CUKE_ARG(1);
  auto& state = cuke::context<openfeature_e2e::State>();
  EXPECT_EQ(state.last_evaluation_value.AsString().value(), expected);
}

WHEN(
    eval_integer_flag,
    "an integer flag with key {string} is evaluated with default value {int}") {
  std::string key = CUKE_ARG(1);
  int64_t default_val = CUKE_ARG(2);
  auto& state = cuke::context<openfeature_e2e::State>();

  state.last_evaluation_value =
      openfeature::Value(state.client->GetIntegerValue(key, default_val));
}

THEN(check_resolved_integer, "the resolved integer value should be {int}") {
  int64_t expected = CUKE_ARG(1);
  auto& state = cuke::context<openfeature_e2e::State>();
  EXPECT_EQ(state.last_evaluation_value.AsInt().value(), expected);
}

WHEN(
    eval_float_flag,
    "a float flag with key {string} is evaluated with default value {double}") {
  std::string key = CUKE_ARG(1);
  double default_val = CUKE_ARG(2);
  auto& state = cuke::context<openfeature_e2e::State>();

  state.last_evaluation_value =
      openfeature::Value(state.client->GetDoubleValue(key, default_val));
}

THEN(check_resolved_float, "the resolved float value should be {double}") {
  double expected = CUKE_ARG(1);
  auto& state = cuke::context<openfeature_e2e::State>();
  EXPECT_DOUBLE_EQ(state.last_evaluation_value.AsDouble().value(), expected);
}

WHEN(
    eval_object_flag_null,
    "an object flag with key {string} is evaluated with a null default value") {
  std::string key = CUKE_ARG(1);
  auto& state = cuke::context<openfeature_e2e::State>();

  state.last_evaluation_value =
      state.client->GetObjectValue(key, openfeature::Value());
}

THEN(check_resolved_object,
     "the resolved object value should be contain fields {string}, {string}, "
     "and {string}, with values {string}, {string} and {int}, respectively") {
  std::string f1 = CUKE_ARG(1);
  std::string f2 = CUKE_ARG(2);
  std::string f3 = CUKE_ARG(3);
  std::string v1_str = CUKE_ARG(4);
  std::string v2 = CUKE_ARG(5);
  int64_t v3 = CUKE_ARG(6);
  auto& state = cuke::context<openfeature_e2e::State>();

  const std::map<std::string, openfeature::Value>* structure =
      state.last_evaluation_value.AsStructure();
  ASSERT_NE(structure, nullptr);

  EXPECT_EQ(structure->at(f1).AsBool().value(), (v1_str == "true"));
  EXPECT_EQ(structure->at(f2).AsString().value(), v2);
  EXPECT_EQ(structure->at(f3).AsInt().value(), v3);
}

WHEN(setup_context,
     "context contains keys {string}, {string}, {string}, {string} with values "
     "{string}, {string}, {int}, {string}") {
  std::string k1 = CUKE_ARG(1);
  std::string k2 = CUKE_ARG(2);
  std::string k3 = CUKE_ARG(3);
  std::string k4 = CUKE_ARG(4);
  std::string v1 = CUKE_ARG(5);
  std::string v2 = CUKE_ARG(6);
  int64_t v3 = CUKE_ARG(7);
  std::string v4 = CUKE_ARG(8);
  auto& state = cuke::context<openfeature_e2e::State>();

  openfeature::EvaluationContext ctx = openfeature::EvaluationContext::Builder()
                                           .WithAttribute(k1, v1)
                                           .WithAttribute(k2, v2)
                                           .WithAttribute(k3, v3)
                                           .WithAttribute(k4, v4)
                                           .build();

  state.context = std::make_unique<openfeature::EvaluationContext>(ctx);
}

WHEN(eval_flag_with_context,
     "a flag with key {string} is evaluated with default value {string}") {
  std::string key = CUKE_ARG(1);
  std::string default_val = CUKE_ARG(2);
  auto& state = cuke::context<openfeature_e2e::State>();

  if (state.context) {
    state.last_evaluation_value = openfeature::Value(
        state.client->GetStringValue(key, default_val, *state.context));
  } else {
    state.last_evaluation_value =
        openfeature::Value(state.client->GetStringValue(key, default_val));
  }
}

THEN(check_resolved_string_response,
     "the resolved string response should be {string}") {
  std::string expected = CUKE_ARG(1);
  auto& state = cuke::context<openfeature_e2e::State>();

  EXPECT_EQ(state.last_evaluation_value.AsString().value(), expected);
}

THEN(check_resolved_flag_empty_ctx,
     "the resolved flag value is {string} when the context is empty") {
  std::string expected = CUKE_ARG(1);
  auto& state = cuke::context<openfeature_e2e::State>();

  openfeature::EvaluationContext empty_ctx =
      openfeature::EvaluationContext::Builder().build();
  std::string actual =
      state.client->GetStringValue("context-aware", "EXTERNAL", empty_ctx);

  EXPECT_EQ(actual, expected);
}

// TODO: Enable more Gherkin scenarios as the SDK functionality expands, e.g.
// around event hooks, detailed evaluation, etc.