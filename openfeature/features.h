#ifndef CPP_SDK_INCLUDE_OPENFEATURE_FEATURES_H_
#define CPP_SDK_INCLUDE_OPENFEATURE_FEATURES_H_

#include <string_view>

#include "openfeature/evaluation_context.h"
#include "openfeature/evaluation_options.h"
#include "openfeature/flag_evaluation_details.h"
#include "openfeature/value.h"

namespace openfeature {

class Features {
 public:
  virtual ~Features() = default;

  virtual bool GetBooleanValue(std::string_view flag_key,
                               bool default_value) = 0;
  virtual bool GetBooleanValue(std::string_view flag_key, bool default_value,
                               const EvaluationContext& ctx) = 0;
  virtual bool GetBooleanValue(std::string_view flag_key, bool default_value,
                               const EvaluationOptions& options) = 0;
  virtual bool GetBooleanValue(std::string_view flag_key, bool default_value,
                               const EvaluationContext& ctx,
                               const EvaluationOptions& options) = 0;
  virtual BoolFlagEvaluationDetails GetBooleanDetails(std::string_view flag_key,
                                                      bool default_value) = 0;
  virtual BoolFlagEvaluationDetails GetBooleanDetails(
      std::string_view flag_key, bool default_value,
      const EvaluationContext& ctx) = 0;
  virtual BoolFlagEvaluationDetails GetBooleanDetails(
      std::string_view flag_key, bool default_value,
      const EvaluationOptions& options) = 0;
  virtual BoolFlagEvaluationDetails GetBooleanDetails(
      std::string_view flag_key, bool default_value,
      const EvaluationContext& ctx, const EvaluationOptions& options) = 0;

  virtual std::string GetStringValue(std::string_view flag_key,
                                     std::string_view default_value) = 0;
  virtual std::string GetStringValue(std::string_view flag_key,
                                     std::string_view default_value,
                                     const EvaluationContext& ctx) = 0;
  virtual std::string GetStringValue(std::string_view flag_key,
                                     std::string_view default_value,
                                     const EvaluationOptions& options) = 0;
  virtual std::string GetStringValue(std::string_view flag_key,
                                     std::string_view default_value,
                                     const EvaluationContext& ctx,
                                     const EvaluationOptions& options) = 0;
  virtual StringFlagEvaluationDetails GetStringDetails(
      std::string_view flag_key, std::string_view default_value) = 0;
  virtual StringFlagEvaluationDetails GetStringDetails(
      std::string_view flag_key, std::string_view default_value,
      const EvaluationContext& ctx) = 0;
  virtual StringFlagEvaluationDetails GetStringDetails(
      std::string_view flag_key, std::string_view default_value,
      const EvaluationOptions& options) = 0;
  virtual StringFlagEvaluationDetails GetStringDetails(
      std::string_view flag_key, std::string_view default_value,
      const EvaluationContext& ctx, const EvaluationOptions& options) = 0;

  virtual int64_t GetIntegerValue(std::string_view flag_key,
                                  int64_t default_value) = 0;
  virtual int64_t GetIntegerValue(std::string_view flag_key,
                                  int64_t default_value,
                                  const EvaluationContext& ctx) = 0;
  virtual int64_t GetIntegerValue(std::string_view flag_key,
                                  int64_t default_value,
                                  const EvaluationOptions& options) = 0;
  virtual int64_t GetIntegerValue(std::string_view flag_key,
                                  int64_t default_value,
                                  const EvaluationContext& ctx,
                                  const EvaluationOptions& options) = 0;
  virtual IntFlagEvaluationDetails GetIntegerDetails(std::string_view flag_key,
                                                     int64_t default_value) = 0;
  virtual IntFlagEvaluationDetails GetIntegerDetails(
      std::string_view flag_key, int64_t default_value,
      const EvaluationContext& ctx) = 0;
  virtual IntFlagEvaluationDetails GetIntegerDetails(
      std::string_view flag_key, int64_t default_value,
      const EvaluationOptions& options) = 0;
  virtual IntFlagEvaluationDetails GetIntegerDetails(
      std::string_view flag_key, int64_t default_value,
      const EvaluationContext& ctx, const EvaluationOptions& options) = 0;

  virtual double GetDoubleValue(std::string_view flag_key,
                                double default_value) = 0;
  virtual double GetDoubleValue(std::string_view flag_key, double default_value,
                                const EvaluationContext& ctx) = 0;
  virtual double GetDoubleValue(std::string_view flag_key, double default_value,
                                const EvaluationOptions& options) = 0;
  virtual double GetDoubleValue(std::string_view flag_key, double default_value,
                                const EvaluationContext& ctx,
                                const EvaluationOptions& options) = 0;
  virtual DoubleFlagEvaluationDetails GetDoubleDetails(
      std::string_view flag_key, double default_value) = 0;
  virtual DoubleFlagEvaluationDetails GetDoubleDetails(
      std::string_view flag_key, double default_value,
      const EvaluationContext& ctx) = 0;
  virtual DoubleFlagEvaluationDetails GetDoubleDetails(
      std::string_view flag_key, double default_value,
      const EvaluationOptions& options) = 0;
  virtual DoubleFlagEvaluationDetails GetDoubleDetails(
      std::string_view flag_key, double default_value,
      const EvaluationContext& ctx, const EvaluationOptions& options) = 0;

  virtual Value GetObjectValue(std::string_view flag_key,
                               Value default_value) = 0;
  virtual Value GetObjectValue(std::string_view flag_key, Value default_value,
                               const EvaluationContext& ctx) = 0;
  virtual Value GetObjectValue(std::string_view flag_key, Value default_value,
                               const EvaluationOptions& options) = 0;
  virtual Value GetObjectValue(std::string_view flag_key, Value default_value,
                               const EvaluationContext& ctx,
                               const EvaluationOptions& options) = 0;
  virtual ObjectFlagEvaluationDetails GetObjectDetails(
      std::string_view flag_key, Value default_value) = 0;
  virtual ObjectFlagEvaluationDetails GetObjectDetails(
      std::string_view flag_key, Value default_value,
      const EvaluationContext& ctx) = 0;
  virtual ObjectFlagEvaluationDetails GetObjectDetails(
      std::string_view flag_key, Value default_value,
      const EvaluationOptions& options) = 0;
  virtual ObjectFlagEvaluationDetails GetObjectDetails(
      std::string_view flag_key, Value default_value,
      const EvaluationContext& ctx, const EvaluationOptions& options) = 0;
};

}  // namespace openfeature

#endif  // CPP_SDK_INCLUDE_OPENFEATURE_FEATURES_H_
