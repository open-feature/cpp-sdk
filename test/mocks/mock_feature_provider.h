#ifndef CPP_SDK_TEST_MOCKS_MOCK_FEATURE_PROVIDER_H_
#define CPP_SDK_TEST_MOCKS_MOCK_FEATURE_PROVIDER_H_

#include <gmock/gmock.h>

#include "openfeature/provider.h"

namespace openfeature {

// Mock class for FeatureProvider to simulate its behavior in tests.
class MockFeatureProvider : public FeatureProvider {
 public:
  MOCK_METHOD(Metadata, GetMetadata, (), (const, override));
  MOCK_METHOD(std::unique_ptr<BoolResolutionDetails>, GetBooleanEvaluation,
              (std::string_view flag, bool default_value,
               const EvaluationContext& ctx),
              (override));
  MOCK_METHOD(std::unique_ptr<StringResolutionDetails>, GetStringEvaluation,
              (std::string_view flag, std::string_view default_value,
               const EvaluationContext& ctx),
              (override));
  MOCK_METHOD(std::unique_ptr<IntResolutionDetails>, GetIntegerEvaluation,
              (std::string_view flag, int64_t default_value,
               const EvaluationContext& ctx),
              (override));
  MOCK_METHOD(std::unique_ptr<DoubleResolutionDetails>, GetDoubleEvaluation,
              (std::string_view flag, double default_value,
               const EvaluationContext& ctx),
              (override));
  MOCK_METHOD(std::unique_ptr<ObjectResolutionDetails>, GetObjectEvaluation,
              (std::string_view flag, Value default_value,
               const EvaluationContext& ctx),
              (override));
  MOCK_METHOD(absl::Status, Init, (const EvaluationContext& ctx), (override));
  MOCK_METHOD(absl::Status, Shutdown, (), (override));

  // TODO: Update mock when FeatureProvider interface is completed
};

}  // namespace openfeature

#endif  // CPP_SDK_TEST_MOCKS_MOCK_FEATURE_PROVIDER_H_