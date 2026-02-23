#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <any>
#include <chrono>
#include <cucumber-cpp/autodetect.hpp>
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

using cucumber::ScenarioScope;
using ::testing::_;
using ::testing::Return;

// Helper to create simple static flags for the InMemoryProvider
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

  // Set up the static flags expected by the basic evaluation tests
  flags["boolean-flag"] = CreateStaticFlag<bool>(true);
  flags["string-flag"] = CreateStaticFlag<std::string>("hi");
  flags["integer-flag"] = CreateStaticFlag<int64_t>(10);
  flags["float-flag"] = CreateStaticFlag<double>(0.5);

  // Object flag setup
  std::map<std::string, openfeature::Value> obj_map;
  obj_map["showImages"] = openfeature::Value(true);
  obj_map["title"] = openfeature::Value("Check out these pics!");
  obj_map["imagesPerPage"] = openfeature::Value(100);
  flags["object-flag"] =
      CreateStaticFlag<openfeature::Value>(openfeature::Value(obj_map));

  // Context-aware flag setup
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
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        return absl::OkStatus();
      }));
  return mock;
}

GIVEN("^a (.*) provider$") {
  REGEX_PARAM(std::string, status_type);
  ScenarioScope<openfeature_e2e::State> state;

  if (status_type == "stable" || status_type == "ready") {
    state->provider = CreateStableProvider();
    openfeature::OpenFeatureAPI::GetInstance().SetProviderAndWait(
        state->provider);
  } else if (status_type == "error") {
    state->provider = CreateMockErrorProvider();
    openfeature::OpenFeatureAPI::GetInstance().SetProviderAndWait(
        state->provider);
  } else if (status_type == "not ready") {
    state->provider = CreateMockNotReadyProvider();
    openfeature::OpenFeatureAPI::GetInstance().SetProvider(state->provider);
  }

  state->client = openfeature::OpenFeatureAPI::GetInstance().GetClient();
}

THEN("^the provider status should be \"([^\"]*)\"$") {
  REGEX_PARAM(std::string, expected_status_str);
  ScenarioScope<openfeature_e2e::State> state;

  openfeature::ProviderStatus actual_status =
      state->client->GetProviderStatus();
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

WHEN(
    "^a boolean flag with key \"([^\"]*)\" is evaluated with default value "
    "\"([^\"]*)\"$") {
  REGEX_PARAM(std::string, key);
  REGEX_PARAM(std::string, default_val_str);
  ScenarioScope<openfeature_e2e::State> state;

  bool default_val = (default_val_str == "true");
  state->last_evaluation_value =
      openfeature::Value(state->client->GetBooleanValue(key, default_val));
}

THEN("^the resolved boolean value should be \"([^\"]*)\"$") {
  REGEX_PARAM(std::string, expected_str);
  ScenarioScope<openfeature_e2e::State> state;
  bool expected = (expected_str == "true");
  EXPECT_EQ(state->last_evaluation_value.AsBool().value(), expected);
}

WHEN(
    "^a string flag with key \"([^\"]*)\" is evaluated with default value "
    "\"([^\"]*)\"$") {
  REGEX_PARAM(std::string, key);
  REGEX_PARAM(std::string, default_val);
  ScenarioScope<openfeature_e2e::State> state;

  state->last_evaluation_value =
      openfeature::Value(state->client->GetStringValue(key, default_val));
}

THEN("^the resolved string value should be \"([^\"]*)\"$") {
  REGEX_PARAM(std::string, expected);
  ScenarioScope<openfeature_e2e::State> state;
  EXPECT_EQ(state->last_evaluation_value.AsString().value(), expected);
}

WHEN(
    "^an integer flag with key \"([^\"]*)\" is evaluated with default value "
    "(\\d+)$") {
  REGEX_PARAM(std::string, key);
  REGEX_PARAM(int64_t, default_val);
  ScenarioScope<openfeature_e2e::State> state;

  state->last_evaluation_value =
      openfeature::Value(state->client->GetIntegerValue(key, default_val));
}

THEN("^the resolved integer value should be (\\d+)$") {
  REGEX_PARAM(int64_t, expected);
  ScenarioScope<openfeature_e2e::State> state;
  EXPECT_EQ(state->last_evaluation_value.AsInt().value(), expected);
}

WHEN(
    "^a float flag with key \"([^\"]*)\" is evaluated with default value "
    "([\\d\\.]+)$") {
  REGEX_PARAM(std::string, key);
  REGEX_PARAM(double, default_val);
  ScenarioScope<openfeature_e2e::State> state;

  state->last_evaluation_value =
      openfeature::Value(state->client->GetDoubleValue(key, default_val));
}

THEN("^the resolved float value should be ([\\d\\.]+)$") {
  REGEX_PARAM(double, expected);
  ScenarioScope<openfeature_e2e::State> state;
  EXPECT_DOUBLE_EQ(state->last_evaluation_value.AsDouble().value(), expected);
}

WHEN(
    "^an object flag with key \"([^\"]*)\" is evaluated with a null default "
    "value$") {
  REGEX_PARAM(std::string, key);
  ScenarioScope<openfeature_e2e::State> state;

  state->last_evaluation_value =
      state->client->GetObjectValue(key, openfeature::Value());
}

THEN(
    "^the resolved object value should be contain fields \"([^\"]*)\", "
    "\"([^\"]*)\", and \"([^\"]*)\", with values \"([^\"]*)\", \"([^\"]*)\" "
    "and (\\d+), respectively$") {
  REGEX_PARAM(std::string, f1);
  REGEX_PARAM(std::string, f2);
  REGEX_PARAM(std::string, f3);
  REGEX_PARAM(std::string, v1_str);
  REGEX_PARAM(std::string, v2);
  REGEX_PARAM(int64_t, v3);
  ScenarioScope<openfeature_e2e::State> state;

  const std::map<std::string, openfeature::Value>* structure =
      state->last_evaluation_value.AsStructure();
  ASSERT_NE(structure, nullptr);

  EXPECT_EQ(structure->at(f1).AsBool().value(), (v1_str == "true"));
  EXPECT_EQ(structure->at(f2).AsString().value(), v2);
  EXPECT_EQ(structure->at(f3).AsInt().value(), v3);
}

WHEN(
    "^context contains keys \"([^\"]*)\", \"([^\"]*)\", \"([^\"]*)\", "
    "\"([^\"]*)\" with values \"([^\"]*)\", \"([^\"]*)\", (\\d+), "
    "\"([^\"]*)\"$") {
  REGEX_PARAM(std::string, k1);
  REGEX_PARAM(std::string, k2);
  REGEX_PARAM(std::string, k3);
  REGEX_PARAM(std::string, k4);
  REGEX_PARAM(std::string, v1);
  REGEX_PARAM(std::string, v2);
  REGEX_PARAM(int64_t, v3);
  REGEX_PARAM(std::string, v4);
  ScenarioScope<openfeature_e2e::State> state;

  openfeature::EvaluationContext ctx = openfeature::EvaluationContext::Builder()
                                           .WithAttribute(k1, v1)
                                           .WithAttribute(k2, v2)
                                           .WithAttribute(k3, v3)
                                           .WithAttribute(k4, v4)
                                           .build();

  state->context = std::make_unique<openfeature::EvaluationContext>(ctx);
}

WHEN(
    "^a flag with key \"([^\"]*)\" is evaluated with default value "
    "\"([^\"]*)\"$") {
  REGEX_PARAM(std::string, key);
  REGEX_PARAM(std::string, default_val);
  ScenarioScope<openfeature_e2e::State> state;

  // Evaluate using the context built in the previous step
  if (state->context) {
    state->last_evaluation_value = openfeature::Value(
        state->client->GetStringValue(key, default_val, *state->context));
  } else {
    state->last_evaluation_value =
        openfeature::Value(state->client->GetStringValue(key, default_val));
  }
}

THEN("^the resolved string response should be \"([^\"]*)\"$") {
  REGEX_PARAM(std::string, expected);
  ScenarioScope<openfeature_e2e::State> state;

  EXPECT_EQ(state->last_evaluation_value.AsString().value(), expected);
}

THEN("^the resolved flag value is \"([^\"]*)\" when the context is empty$") {
  REGEX_PARAM(std::string, expected);
  ScenarioScope<openfeature_e2e::State> state;

  // Evaluate context-aware flag with an empty context
  openfeature::EvaluationContext empty_ctx =
      openfeature::EvaluationContext::Builder().build();
  std::string actual =
      state->client->GetStringValue("context-aware", "EXTERNAL", empty_ctx);

  EXPECT_EQ(actual, expected);
}
