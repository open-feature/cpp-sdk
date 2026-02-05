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
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::StrictMock;

class ClientAPITest : public ::testing::Test {
 protected:
  void SetUp() override {
    // Reset the Global Context to a clean state before each test.
    GlobalContextManager::GetInstance().SetGlobalEvaluationContext(
        EvaluationContext::Builder().build());
  }
  ProviderRepository repo_;
};

// Test that the constructor correctly sets the domain in the metadata.
TEST_F(ClientAPITest, ConstructorSetsDomainMetadata) {
  std::string domain = "test-domain";
  ClientAPI client(repo_, domain);

  Metadata metadata = client.GetMetadata();
  EXPECT_EQ(metadata.name, domain);
}

// Test that the provider status is Ready by default.
TEST_F(ClientAPITest, GetProviderStatusDefaultsToReady) {
  ClientAPI client(repo_, "test-domain");
  EXPECT_EQ(client.GetProviderStatus(), ProviderStatus::kReady);
}

// Test setting and getting the EvaluationContext.
TEST_F(ClientAPITest, SetAndGetEvaluationContext) {
  ClientAPI client(repo_, "test-domain");
  EvaluationContext ctx = EvaluationContext::Builder().build();

  // Verify we can set the context without error.
  EXPECT_NO_THROW(client.SetEvaluationContext(ctx));
}

// Test that GetBooleanValue returns the default value when using the default
// provider.
TEST_F(ClientAPITest, GetBooleanValueReturnsDefaultWithNoopProvider) {
  ClientAPI client(repo_, "test-domain");
  std::string flag_key = "my-boolean-flag";

  EXPECT_TRUE(client.GetBooleanValue(flag_key, true));

  EXPECT_FALSE(client.GetBooleanValue(flag_key, false));
}

// Test that GetStringValue returns the default value when using the default
// provider.
TEST_F(ClientAPITest, GetStringValueReturnsDefaultWithNoopProvider) {
  ClientAPI client(repo_, "test-domain");
  std::string flag_key = "my-string-flag";
  EXPECT_EQ(client.GetStringValue(flag_key, "default"), "default");
}

// Test that GetIntegerValue returns the default value when using the default
// provider.
TEST_F(ClientAPITest, GetIntegerValueReturnsDefaultWithNoopProvider) {
  ClientAPI client(repo_, "test-domain");
  std::string flag_key = "my-integer-flag";
  EXPECT_EQ(client.GetIntegerValue(flag_key, 42), 42);
}

// Test that GetDoubleValue returns the default value when using the default
// provider.
TEST_F(ClientAPITest, GetDoubleValueReturnsDefaultWithNoopProvider) {
  ClientAPI client(repo_, "test-domain");
  std::string flag_key = "my-double-flag";
  EXPECT_DOUBLE_EQ(client.GetDoubleValue(flag_key, 3.14), 3.14);
}

// Test that GetObjectValue returns the default value when using the default
// provider.
TEST_F(ClientAPITest, GetObjectValueReturnsDefaultWithNoopProvider) {
  ClientAPI client(repo_, "test-domain");
  std::string flag_key = "my-object-flag";
  EXPECT_EQ(client.GetObjectValue(flag_key, Value(1)), Value(1));
}

// Test GetBooleanValue with an EvaluationContext passed in.
TEST_F(ClientAPITest, GetBooleanValueWithContextReturnsDefault) {
  ClientAPI client(repo_, "test-domain");
  EvaluationContext ctx = EvaluationContext::Builder().build();
  std::string flag_key = "my-boolean-flag";

  EXPECT_TRUE(client.GetBooleanValue(flag_key, true, ctx));
  EXPECT_FALSE(client.GetBooleanValue(flag_key, false, ctx));
}

// Test GetStringValue with an EvaluationContext passed in.
TEST_F(ClientAPITest, GetStringValueWithContextReturnsDefault) {
  ClientAPI client(repo_, "test-domain");
  EvaluationContext ctx = EvaluationContext::Builder().build();
  std::string flag_key = "my-string-flag";
  EXPECT_EQ(client.GetStringValue(flag_key, "default", ctx), "default");
}

// Test that GetIntegerValue with an EvaluationContext passed in.
TEST_F(ClientAPITest, GetIntegerValueWithContextReturnsDefault) {
  ClientAPI client(repo_, "test-domain");
  EvaluationContext ctx = EvaluationContext::Builder().build();
  std::string flag_key = "my-integer-flag";
  EXPECT_EQ(client.GetIntegerValue(flag_key, 42, ctx), 42);
}

// Test that GetDoubleValue with an EvaluationContext passed in.
TEST_F(ClientAPITest, GetDoubleValueWithContextReturnsDefault) {
  ClientAPI client(repo_, "test-domain");
  EvaluationContext ctx = EvaluationContext::Builder().build();
  std::string flag_key = "my-double-flag";
  EXPECT_DOUBLE_EQ(client.GetDoubleValue(flag_key, 3.14, ctx), 3.14);
}

// Test that GetObjectValue with an EvaluationContext passed in.
TEST_F(ClientAPITest, GetObjectValueWithContextReturnsDefault) {
  ClientAPI client(repo_, "test-domain");
  EvaluationContext ctx = EvaluationContext::Builder().build();
  std::string flag_key = "my-object-flag";
  EXPECT_EQ(client.GetObjectValue(flag_key, Value(1), ctx), Value(1));
}

// Test context merging logic indirectly.
TEST_F(ClientAPITest, GetBooleanValueSafeWithMergedContexts) {
  EvaluationContext global_ctx = EvaluationContext::Builder().build();
  GlobalContextManager::GetInstance().SetGlobalEvaluationContext(global_ctx);

  ClientAPI client(repo_, "test-domain");
  EvaluationContext client_ctx = EvaluationContext::Builder().build();
  client.SetEvaluationContext(client_ctx);

  EvaluationContext invocation_ctx = EvaluationContext::Builder().build();

  // This call forces a merge of Global + Client + Invocation contexts.
  // We expect the NoopProvider to handle the result gracefully (return
  // default).
  EXPECT_TRUE(client.GetBooleanValue("flag", true, invocation_ctx));
}

// Test behavior when the domain is empty.
TEST_F(ClientAPITest, WorksWithEmptyDomain) {
  ClientAPI client(repo_, "");
  EXPECT_EQ(client.GetMetadata().name, "");
  EXPECT_TRUE(client.GetBooleanValue("flag", true));
}

// TODO: If ClientAPI is refactored to allow injecting a MockFeatureProvider
// (e.g. by passing the OpenFeatureAPI's repository or via constructor),
// add tests here to verify that the MockProvider receives the correct
// flag key and merged EvaluationContext.