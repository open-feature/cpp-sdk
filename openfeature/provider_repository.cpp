#include "openfeature/provider_repository.h"

#include <iostream>
#include <mutex>
#include <thread>

#include "openfeature/noop_provider.h"

namespace openfeature {

ProviderRepository::ProviderRepository() {
  // Initialize with a NoopProvider by default, which is always ready.
  auto noop_provider = std::make_shared<NoopProvider>();
  auto status_manager = FeatureProviderStatusManager::Create(noop_provider);
  if (status_manager.ok()) {
    std::unique_lock<std::shared_mutex> lock(repo_mutex_);
    default_manager_ = std::move(status_manager.value());
    default_manager_->SetStatus(ProviderStatus::kReady);
  }
}

ProviderRepository::~ProviderRepository() { Shutdown(); }

std::shared_ptr<FeatureProviderStatusManager>
ProviderRepository::GetFeatureProviderStatusManager(std::string_view domain) {
  std::shared_lock<std::shared_mutex> lock(repo_mutex_);

  if (domain.empty()) {
    return default_manager_;
  }

  // Look up domain in map.
  auto it = provider_manager_.find(std::string(domain));
  if (it != provider_manager_.end()) {
    return it->second;
  }

  // Fallback to default if no provider was found.
  return default_manager_;
}

std::shared_ptr<FeatureProvider> ProviderRepository::GetProvider(
    std::string_view domain) {
  auto manager = GetFeatureProviderStatusManager(domain);

  if (manager) {
    return manager->GetProvider();
  }
  return nullptr;
}

void ProviderRepository::SetProvider(std::shared_ptr<FeatureProvider> provider,
                                     const EvaluationContext& ctx,
                                     bool waitForInit) {
  if (!provider) {
    std::cerr << "Provider cannot be null" << std::endl;
    return;
  }
  PrepareAndInitializeProvider(std::nullopt, std::move(provider), ctx,
                               waitForInit);
}

void ProviderRepository::SetProvider(std::string_view domain,
                                     std::shared_ptr<FeatureProvider> provider,
                                     const EvaluationContext& ctx,
                                     bool waitForInit) {
  if (!provider) {
    std::cerr << "Provider cannot be null" << std::endl;
    return;
  }

  if (domain.empty()) {
    SetProvider(std::move(provider), ctx, waitForInit);
    return;
  }

  PrepareAndInitializeProvider(std::string(domain), std::move(provider), ctx,
                               waitForInit);
}

ProviderStatus ProviderRepository::GetProviderStatus(std::string_view domain) {
  auto manager = GetFeatureProviderStatusManager(domain);
  if (manager) {
    return manager->GetStatus();
  }
  return ProviderStatus::kNotReady;
}

void ProviderRepository::Shutdown() {
  std::unique_lock<std::shared_mutex> lock(repo_mutex_);

  if (default_manager_) {
    default_manager_->Shutdown();
    default_manager_.reset();
  }

  for (auto& pair : provider_manager_) {
    if (pair.second->GetStatus() != ProviderStatus::kNotReady) {
      pair.second->Shutdown();
    }
  }
  provider_manager_.clear();
}

void ProviderRepository::PrepareAndInitializeProvider(
    const std::optional<std::string> domain,
    std::shared_ptr<FeatureProvider> new_provider, const EvaluationContext& ctx,
    bool waitForInit) {
  std::shared_ptr<FeatureProviderStatusManager> new_status_manager;
  std::shared_ptr<FeatureProviderStatusManager> old_status_manager;

  {  // Scoping for the unique_lock.
    std::unique_lock<std::shared_mutex> lock(repo_mutex_);

    auto existing_manager = GetExistingStatusManagerForProvider(new_provider);
    if (!existing_manager) {
      auto manager = FeatureProviderStatusManager::Create(new_provider);
      if (!manager.ok()) {
        std::cerr << "Failed to create FeatureProviderStatusManager: "
                  << manager.status() << std::endl;
        return;
      }
      new_status_manager = std::move(manager.value());
    } else {
      new_status_manager = existing_manager;
    }

    if (domain) {
      // Setting a named provider.
      auto it = provider_manager_.find(domain.value());
      if (it != provider_manager_.end()) {
        old_status_manager = it->second;
      }
      provider_manager_[domain.value()] = new_status_manager;
    } else {
      // Setting the default provider.
      old_status_manager = default_manager_;
      default_manager_ = new_status_manager;
    }
  }  // Release the lock before running initialization logic.

  // Decide whether to initialize sync or async.
  if (waitForInit) {
    InitializeProvider(new_status_manager, old_status_manager, ctx);
  } else {
    std::thread([this, new_status_manager, old_status_manager, ctx] {
      InitializeProvider(new_status_manager, old_status_manager, ctx);
    }).detach();
  }
}

void ProviderRepository::InitializeProvider(
    std::shared_ptr<FeatureProviderStatusManager> new_status_manager,
    std::shared_ptr<FeatureProviderStatusManager> old_status_manager,
    const EvaluationContext& ctx) {
  if (new_status_manager->GetStatus() == ProviderStatus::kNotReady) {
    new_status_manager->Init(ctx);
  }

  if (new_status_manager->GetStatus() == ProviderStatus::kReady) {
    ShutdownOldProvider(old_status_manager);
  }
}

void ProviderRepository::ShutdownOldProvider(
    std::shared_ptr<FeatureProviderStatusManager> old_status_manager) {
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
  for (const auto& pair : provider_manager_) {
    if (pair.second == manager) {
      return true;
    }
  }
  return false;
}

}  // namespace openfeature
