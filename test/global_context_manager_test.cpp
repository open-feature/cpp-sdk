#include "openfeature/global_context_manager.h"

#include <gtest/gtest.h>

#include <atomic>
#include <thread>
#include <vector>

#include "openfeature/evaluation_context.h"

namespace openfeature {

class GlobalContextManagerTest : public ::testing::Test {
 protected:
  // Reset to a clean state before every test.
  void SetUp() override {
    GlobalContextManager::GetInstance().SetGlobalEvaluationContext(
        EvaluationContext::Builder().Build());
  }
};

TEST_F(GlobalContextManagerTest, ReturnsSameInstance) {
  GlobalContextManager& instance1 = GlobalContextManager::GetInstance();
  GlobalContextManager& instance2 = GlobalContextManager::GetInstance();

  EXPECT_EQ(&instance1, &instance2);
}

TEST_F(GlobalContextManagerTest, SetAndGetContext) {
  GlobalContextManager& manager = GlobalContextManager::GetInstance();
  EvaluationContext input_ctx = EvaluationContext::Builder().Build();

  EXPECT_NO_THROW(manager.SetGlobalEvaluationContext(input_ctx));

  EvaluationContext output_ctx = manager.GetGlobalEvaluationContext();
  EXPECT_NO_THROW(output_ctx = manager.GetGlobalEvaluationContext());

  // NOTE: Since EvaluationContext is currently an empty class,
  // we cannot assert EQ or check member values.
  // Currently, the test simply proves the API calls succeed and copy semantics
  // work.

  // TODO: Add assertions.
}

TEST_F(GlobalContextManagerTest, ThreadSafetyStressTest) {
  GlobalContextManager& manager = GlobalContextManager::GetInstance();
  std::atomic<bool> stop{false};

  // Writer Thread: Continuously updates the context.
  std::thread writer([&]() {
    while (!stop) {
      EvaluationContext ctx = EvaluationContext::Builder().Build();
      // In a real scenario, we would populate ctx with different data here.
      manager.SetGlobalEvaluationContext(ctx);
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
  });

  // Reader Threads: Continuously read the context.
  constexpr int kReaderThreadCount = 10;
  std::vector<std::thread> readers;
  readers.reserve(kReaderThreadCount);
  for (int index = 0; index < kReaderThreadCount; ++index) {
    readers.emplace_back([&]() {
      while (!stop) {
        // Just calling the getter to ensure locks work and no race conditions
        // occur resulting in a crash (segfault).
        EvaluationContext ctx = manager.GetGlobalEvaluationContext();

        // Prevent optimization from removing the call.
        volatile size_t size = sizeof(ctx);
        (void)size;
      }
    });
  }

  // Let the chaos run for a short duration.
  constexpr int kChaosDurationMs = 100;
  std::this_thread::sleep_for(std::chrono::milliseconds(kChaosDurationMs));

  stop = true;
  writer.join();
  for (auto& reader_thread : readers) {
    reader_thread.join();
  }
}
}  // namespace openfeature