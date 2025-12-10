#include "openfeature/feature_provider_status_manager.h"

#include <iostream>
#include <memory>
#include <mutex>
#include <utility>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "openfeature/evaluation_context.h"
#include "openfeature/provider.h"
#include "openfeature/provider_status.h"

namespace openfeature {

FeatureProviderStatusManager::FeatureProviderStatusManager(
    std::shared_ptr<FeatureProvider> provider)
    : provider_(std::move(provider)), status_(ProviderStatus::kNotReady) {}

absl::StatusOr<std::unique_ptr<FeatureProviderStatusManager>>
FeatureProviderStatusManager::Create(
    std::shared_ptr<FeatureProvider> provider) {
  if (!provider) {
    return absl::InvalidArgumentError("Provider cannot be null");
  }
  return std::unique_ptr<FeatureProviderStatusManager>(
      new FeatureProviderStatusManager(std::move(provider)));
}

void FeatureProviderStatusManager::Init(const EvaluationContext& ctx) {
  if (!provider_) {
    std::cerr << "Provider is null, cannot initialize." << std::endl;
    SetStatus(ProviderStatus::kError);
    return;
  }

  absl::Status state_handler = provider_->Init(ctx);

  if (state_handler.ok()) {
    SetStatus(ProviderStatus::kReady);
  } else {
    std::cerr << "Provider initialization failed: " << state_handler
              << std::endl;
    SetStatus(ProviderStatus::kError);
  }
}

void FeatureProviderStatusManager::Shutdown() {
  if (!provider_) return;

  absl::Status state_handler = provider_->Shutdown();

  if (!state_handler.ok()) {
    // TODO: Handle shutdown according to specs.
    std::cerr << "Provider shutdown failed: " << state_handler << std::endl;
  }
  // After shutdown, the provider is no longer usable.
  SetStatus(ProviderStatus::kNotReady);
}

void FeatureProviderStatusManager::SetStatus(ProviderStatus status) {
  // Block all readers for the duration of the update,
  // preventing race conditions
  std::lock_guard<std::mutex> lock(status_mutex_);
  status_ = status;
}

ProviderStatus FeatureProviderStatusManager::GetStatus() const {
  // We allow multiple threads to read the status simultaneously.
  // without blocking each other.
  std::lock_guard<std::mutex> lock(status_mutex_);
  return status_;
}

std::shared_ptr<FeatureProvider> FeatureProviderStatusManager::GetProvider()
    const {
  return provider_;
}

}  // namespace openfeature