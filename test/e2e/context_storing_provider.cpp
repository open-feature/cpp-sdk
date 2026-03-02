#include "test/e2e/context_storing_provider.h"

#include <gtest/gtest.h>

#include <cucumber-cpp/autodetect.hpp>
#include <optional>

#include "openfeature/flag_metadata.h"
#include "openfeature/reason.h"
#include "openfeature/value.h"

using cucumber::ScenarioScope;

namespace openfeature_e2e {

openfeature::Metadata ContextStoringProvider::GetMetadata() const {
  return openfeature::Metadata{"ContextStoringProvider"};
}

std::unique_ptr<openfeature::BoolResolutionDetails>
ContextStoringProvider::GetBooleanEvaluation(
    std::string_view key, bool default_value,
    const openfeature::EvaluationContext& ctx) {
  last_ctx = ctx;

  return std::make_unique<openfeature::BoolResolutionDetails>(
      default_value, openfeature::Reason::kDefault, "default-variant",
      openfeature::FlagMetadata{}, std::nullopt, "");
}

std::unique_ptr<openfeature::StringResolutionDetails>
ContextStoringProvider::GetStringEvaluation(
    std::string_view key, std::string_view default_value,
    const openfeature::EvaluationContext& ctx) {
  last_ctx = ctx;
  std::string default_str(default_value);
  return std::make_unique<openfeature::StringResolutionDetails>(
      default_str, openfeature::Reason::kDefault, "default-variant",
      openfeature::FlagMetadata{}, std::nullopt, "");
}

std::unique_ptr<openfeature::IntResolutionDetails>
ContextStoringProvider::GetIntegerEvaluation(
    std::string_view key, int64_t default_value,
    const openfeature::EvaluationContext& ctx) {
  last_ctx = ctx;

  return std::make_unique<openfeature::IntResolutionDetails>(
      default_value, openfeature::Reason::kDefault, "default-variant",
      openfeature::FlagMetadata{}, std::nullopt, "");
}

std::unique_ptr<openfeature::DoubleResolutionDetails>
ContextStoringProvider::GetDoubleEvaluation(
    std::string_view key, double default_value,
    const openfeature::EvaluationContext& ctx) {
  last_ctx = ctx;

  return std::make_unique<openfeature::DoubleResolutionDetails>(
      default_value, openfeature::Reason::kDefault, "default-variant",
      openfeature::FlagMetadata{}, std::nullopt, "");
}

std::unique_ptr<openfeature::ObjectResolutionDetails>
ContextStoringProvider::GetObjectEvaluation(
    std::string_view key, const openfeature::Value default_value,
    const openfeature::EvaluationContext& ctx) {
  last_ctx = ctx;

  return std::make_unique<openfeature::ObjectResolutionDetails>(
      default_value, openfeature::Reason::kDefault, "default-variant",
      openfeature::FlagMetadata{}, std::nullopt, "");
}

}  // namespace openfeature_e2e
