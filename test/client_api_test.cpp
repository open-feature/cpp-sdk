#include "openfeature/client_api.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <string>

#include "absl/status/status.h"
#include "mocks/mock_feature_provider.h"
#include "openfeature/evaluation_context.h"
#include "openfeature/global_context_manager.h"
#include "openfeature/provider_status.h"

using namespace openfeature;
using ::testing::_;
using ::testing::Return;

class ClientAPITest : public ::testing::Test {
 protected:
  void SetUp() override {
    // Reset the Global Context to a clean state before each test.
    GlobalContextManager::GetInstance().SetGlobalEvaluationContext(
        EvaluationContext{});
  }
};

// Test that the constructor correctly sets the domain in the metadata.
TEST_F(ClientAPITest, ConstructorSetsDomainMetadata) {
  std::string domain = "test-domain";
  ClientAPI client(domain);

  Metadata metadata = client.GetMetadata();
  EXPECT_EQ(metadata.name, domain);
}

// Test that the provider status is Ready by default.
TEST_F(ClientAPITest, GetProviderStatusDefaultsToReady) {
  ClientAPI client("test-domain");
  EXPECT_EQ(client.GetProviderStatus(), ProviderStatus::kReady);
}

// Test setting and getting the EvaluationContext.
TEST_F(ClientAPITest, SetAndGetEvaluationContext) {
  ClientAPI client("test-domain");
  EvaluationContext ctx;

  // Verify we can set the context without error.
  EXPECT_NO_THROW(client.SetEvaluationContext(ctx));

  // Verify we can retrieve it.
  // Note: Since EvaluationContext is currently empty,
  // we are primarily testing that the API calls function correctly.
  EvaluationContext retrieved_ctx = client.GetEvaluationContext();
}

// Test that GetBooleanValue returns the default value when using the default
// provider.
TEST_F(ClientAPITest, GetBooleanValueReturnsDefaultWithNoopProvider) {
  ClientAPI client("test-domain");
  std::string flag_key = "my-boolean-flag";

  EXPECT_TRUE(client.GetBooleanValue(flag_key, true));

  EXPECT_FALSE(client.GetBooleanValue(flag_key, false));
}

// Test GetBooleanValue with an EvaluationContext passed in.
TEST_F(ClientAPITest, GetBooleanValueWithContextReturnsDefault) {
  ClientAPI client("test-domain");
  EvaluationContext ctx;
  std::string flag_key = "my-boolean-flag";

  EXPECT_TRUE(client.GetBooleanValue(flag_key, true, ctx));
  EXPECT_FALSE(client.GetBooleanValue(flag_key, false, ctx));
}

// Test context merging logic indirectly.
TEST_F(ClientAPITest, GetBooleanValueSafeWithMergedContexts) {
  EvaluationContext global_ctx;
  GlobalContextManager::GetInstance().SetGlobalEvaluationContext(global_ctx);

  ClientAPI client("test-domain");
  EvaluationContext client_ctx;
  client.SetEvaluationContext(client_ctx);

  EvaluationContext invocation_ctx;

  // This call forces a merge of Global + Client + Invocation contexts.
  // We expect the NoopProvider to handle the result gracefully (return
  // default).
  EXPECT_TRUE(client.GetBooleanValue("flag", true, invocation_ctx));
}

// Test behavior when the domain is empty.
TEST_F(ClientAPITest, WorksWithEmptyDomain) {
  ClientAPI client("");
  EXPECT_EQ(client.GetMetadata().name, "");
  EXPECT_TRUE(client.GetBooleanValue("flag", true));
}

// TODO: If ClientAPI is refactored to allow injecting a MockFeatureProvider
// (e.g. by passing the OpenFeatureAPI's repository or via constructor),
// add tests here to verify that the MockProvider receives the correct
// flag key and merged EvaluationContext.