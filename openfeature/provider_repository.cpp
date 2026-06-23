#include "openfeature/provider_repository.h"

#include <algorithm>
#include <iostream>

#include "absl/status/statusor.h"
#include "openfeature/noop_provider.h"

namespace openfeature {

ProviderRepository::ProviderRepository() {
  std::shared_ptr<openfeature::NoopProvider> noop_provider =
      std::make_shared<NoopProvider>();
  absl::StatusOr<std::unique_ptr<FeatureProviderStatusManager>> status_manager =
      FeatureProviderStatusManager::Create(noop_provider);
  if (status_manager.ok()) {
    std::unique_lock<std::shared_mutex> lock(repo_mutex_);
    default_manager_ = std::move(status_manager.value());
    default_manager_->SetStatus(ProviderStatus::kReady);
  }
}

ProviderRepository::~ProviderRepository() { Shutdown(); }

std::shared_ptr<FeatureProviderStatusManager>
ProviderRepository::GetFeatureProviderStatusManager(
    std::string_view domain) const {
  std::shared_lock<std::shared_mutex> lock(repo_mutex_);

  if (domain.empty()) {
    return default_manager_;
  }

  auto provider_manager_it = provider_manager_.find(std::string(domain));
  if (provider_manager_it != provider_manager_.end()) {
    return provider_manager_it->second;
  }

  return default_manager_;
}

std::shared_ptr<FeatureProvider> ProviderRepository::GetProvider(
    std::string_view domain) const {
  std::shared_ptr<FeatureProviderStatusManager> manager =
      GetFeatureProviderStatusManager(domain);

  if (manager) {
    return manager->GetProvider();
  }
  return nullptr;
}

void ProviderRepository::SetProvider(
    const std::shared_ptr<FeatureProvider>& provider,
    const EvaluationContext& ctx, bool wait_for_init) {
  if (!provider) {
    std::cerr << "Provider cannot be null\n";
    return;
  }
  PrepareAndInitializeProvider(std::nullopt, provider, ctx, wait_for_init);
}

void ProviderRepository::SetProvider(
    std::string_view domain, const std::shared_ptr<FeatureProvider>& provider,
    const EvaluationContext& ctx, bool wait_for_init) {
  if (!provider) {
    std::cerr << "Provider cannot be null\n";
    return;
  }

  if (domain.empty()) {
    SetProvider(provider, ctx, wait_for_init);
    return;
  }

  PrepareAndInitializeProvider(std::string(domain), provider, ctx,
                               wait_for_init);
}

ProviderStatus ProviderRepository::GetProviderStatus(
    std::string_view domain) const {
  std::shared_ptr<FeatureProviderStatusManager> manager =
      GetFeatureProviderStatusManager(domain);
  if (manager) {
    return manager->GetStatus();
  }
  return ProviderStatus::kNotReady;
}

void ProviderRepository::Shutdown() {
  {
    std::scoped_lock lock(threads_mutex_);
    for (std::thread& thread : initialization_threads_) {
      if (thread.joinable()) {
        thread.join();
      }
    }
    initialization_threads_.clear();
  }

  std::unique_lock<std::shared_mutex> lock(repo_mutex_);

  if (default_manager_) {
    default_manager_->Shutdown();
    default_manager_.reset();
  }

  for (std::pair<const std::string,
                 std::shared_ptr<FeatureProviderStatusManager>>& pair :
       provider_manager_) {
    // Check if the provider is still active before shutting down.
    if (pair.second->GetStatus() != ProviderStatus::kNotReady) {
      pair.second->Shutdown();
    }
  }
  provider_manager_.clear();

  // Re-initialize to the default state after shutting down
  std::shared_ptr<openfeature::NoopProvider> noop_provider =
      std::make_shared<NoopProvider>();
  absl::StatusOr<std::unique_ptr<FeatureProviderStatusManager>> status_manager =
      FeatureProviderStatusManager::Create(noop_provider);
  if (status_manager.ok()) {
    default_manager_ = std::move(status_manager.value());
    default_manager_->SetStatus(ProviderStatus::kReady);
  }
}

void ProviderRepository::PrepareAndInitializeProvider(
    const std::optional<std::string>& domain,
    const std::shared_ptr<FeatureProvider>& new_provider,
    const EvaluationContext& ctx, bool wait_for_init) {
  std::shared_ptr<FeatureProviderStatusManager> new_status_manager;
  std::shared_ptr<FeatureProviderStatusManager> old_status_manager;

  {  // Scoping for the unique_lock.
    std::unique_lock<std::shared_mutex> lock(repo_mutex_);

    std::shared_ptr<FeatureProviderStatusManager> existing_manager =
        GetExistingStatusManagerForProvider(new_provider);
    if (!existing_manager) {
      absl::StatusOr<std::unique_ptr<FeatureProviderStatusManager>> manager =
          FeatureProviderStatusManager::Create(new_provider);
      if (!manager.ok()) {
        std::cerr << "Failed to create FeatureProviderStatusManager: "
                  << manager.status() << "\n";
        return;
      }
      new_status_manager = std::move(manager.value());
    } else {
      new_status_manager = existing_manager;
    }

    if (domain) {
      // Setting a named provider.
      auto provider_manager_it = provider_manager_.find(domain.value());
      if (provider_manager_it != provider_manager_.end()) {
        old_status_manager = provider_manager_it->second;
      }
      provider_manager_[domain.value()] = new_status_manager;
    } else {
      // Setting the default provider.
      old_status_manager = default_manager_;
      default_manager_ = new_status_manager;
    }
  }  // Release the lock before running initialization logic.

  if (wait_for_init) {
    InitializeProvider(new_status_manager, old_status_manager, ctx);
  } else {
    std::scoped_lock lock(threads_mutex_);
    initialization_threads_.emplace_back(
        [this, new_status_manager, old_status_manager, ctx] {
          InitializeProvider(new_status_manager, old_status_manager, ctx);
        });
  }
}

void ProviderRepository::InitializeProvider(
    const std::shared_ptr<FeatureProviderStatusManager>& new_status_manager,
    const std::shared_ptr<FeatureProviderStatusManager>& old_status_manager,
    const EvaluationContext& ctx) {
  if (new_status_manager->GetStatus() == ProviderStatus::kNotReady) {
    new_status_manager->Init(ctx);
  }

  ShutdownOldProvider(old_status_manager);
}

void ProviderRepository::ShutdownOldProvider(
    const std::shared_ptr<FeatureProviderStatusManager>& old_status_manager) {
  if (old_status_manager) {
    std::shared_lock<std::shared_mutex> lock(repo_mutex_);

    if (!IsStatusManagerRegistered(old_status_manager)) {
      old_status_manager->Shutdown();
    }
  }
}

std::shared_ptr<FeatureProviderStatusManager>
ProviderRepository::GetExistingStatusManagerForProvider(
    const std::shared_ptr<FeatureProvider>& provider) {
  if (default_manager_ && default_manager_->GetProvider() == provider) {
    return default_manager_;
  }

  for (const auto& pair : provider_manager_) {
    if (pair.second && pair.second->GetProvider() == provider) {
      return pair.second;
    }
  }
  return nullptr;
}

bool ProviderRepository::IsStatusManagerRegistered(
    const std::shared_ptr<FeatureProviderStatusManager>& manager) {
  if (default_manager_ == manager) {
    return true;
  }
  return std::any_of(
      provider_manager_.begin(), provider_manager_.end(),
      [&manager](const auto& pair) { return pair.second == manager; });
}

}  // namespace openfeature
