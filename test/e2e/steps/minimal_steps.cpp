#include <gtest/gtest.h>

#include <cucumber-cpp/autodetect.hpp>
#include <iostream>
#include <memory>
#include <string>

#include "openfeature/client.h"
#include "openfeature/evaluation_context.h"
#include "openfeature/openfeature_api.h"
#include "openfeature/provider.h"
#include "openfeature/provider_repository.h"
#include "openfeature/resolution_details.h"
#include "test/e2e/context_storing_provider.h"
#include "test/e2e/state.h"

GIVEN("^a stable provider with retrievable context is registered$") {
  std::cout << "Registering provider..."<<std::endl;
}