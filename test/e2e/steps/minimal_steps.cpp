#include <gtest/gtest.h>

#include <cucumber-cpp/autodetect.hpp>
#include <iostream>
#include <map>
#include <string>

// This include might not be strictly necessary if using autodetect.hpp,
// but it's good for clarity if you need specific internal components.
// #include <cucumber-cpp/internal/Scenario.hpp> // Often not needed for basic
// World access

// OpenFeature C++ SDK includes
#include "openfeature/client.h"
#include "openfeature/evaluation_context.h"
#include "openfeature/openfeature_api.h"
#include "openfeature/provider.h"
#include "openfeature/provider_repository.h"  // For SetProviderAndWait
#include "openfeature/resolution_details.h"

// Custom e2e testing components
#include "test/e2e/context_storing_provider.h"
#include "test/e2e/state.h"

// Define a "World" object that will be instantiated for each scenario.
// This allows sharing state between steps in a scenario.
struct ScenarioWorld {
  openfeature_e2e::ScenarioState state;
};

// IMPORTANT: Declare and define your World object for Cucumber-Cpp
// This makes World::instance() available and correctly typed.
// Place this immediately after the ScenarioWorld struct definition.
CUCUMBER_CPP_DECLARE_AND_DEFINE_WORLD(ScenarioWorld);

// --- GIVEN Steps ---

GIVEN("^a stable provider with retrievable context is registered$") {
  // Access the shared state for this scenario
  auto& state = World::instance()->state;

  // Equivalent to Java's setup()
  state.provider = std::make_shared<openfeature_e2e::ContextStoringProvider>();
  openfeature::OpenFeatureAPI::GetInstance().SetProviderAndWait(state.provider);
  state.client = openfeature::OpenFeatureAPI::GetInstance().GetClient();
  // In C++, the transaction context propagation is usually handled differently
  // or isn't a direct API call like in Java. For now, we omit the
  // ThreadLocalTransactionContextPropagator equivalent.
}
