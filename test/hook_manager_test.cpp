#include "openfeature/hook_manager.h"

#include <gtest/gtest.h>

#include <atomic>
#include <memory>
#include <thread>
#include <vector>

#include "openfeature/general_hook.h"
#include "openfeature/hook.h"

namespace openfeature {
namespace {

class DummyTestHook1 : public BoolHook {};
class DummyTestHook2 : public StringHook {};
class DummyTestHook3 : public IntHook {};

class HookManagerTest : public ::testing::Test {
 protected:
  void SetUp() override { HookManager::GetInstance().ClearHooks(); }

  void TearDown() override { HookManager::GetInstance().ClearHooks(); }
};

TEST_F(HookManagerTest, ReturnsSameSingletonInstance) {
  HookManager& instance1 = HookManager::GetInstance();
  HookManager& instance2 = HookManager::GetInstance();

  EXPECT_EQ(&instance1, &instance2);
}

TEST_F(HookManagerTest, InitialStateHasEmptyHooks) {
  EXPECT_TRUE(HookManager::GetInstance().GetHooks()->empty());
}

TEST_F(HookManagerTest, AddHookAppendsSingleHook) {
  auto& manager = HookManager::GetInstance();
  std::shared_ptr<GeneralHook> hook1 = std::make_shared<DummyTestHook1>();

  manager.AddHook(hook1);

  auto hooks = manager.GetHooks();
  ASSERT_EQ(hooks->size(), 1);
  EXPECT_EQ((*hooks)[0], hook1);
}

TEST_F(HookManagerTest, AddHooksAppendsMultipleHooksAndPreservesOrder) {
  auto& manager = HookManager::GetInstance();
  std::shared_ptr<GeneralHook> hook1 = std::make_shared<DummyTestHook1>();
  std::shared_ptr<GeneralHook> hook2 = std::make_shared<DummyTestHook2>();

  manager.AddHooks({hook1, hook2});

  auto hooks = manager.GetHooks();
  ASSERT_EQ(hooks->size(), 2);
  EXPECT_EQ((*hooks)[0], hook1);
  EXPECT_EQ((*hooks)[1], hook2);

  std::shared_ptr<GeneralHook> hook3 = std::make_shared<DummyTestHook3>();
  manager.AddHook(hook3);

  hooks = manager.GetHooks();
  ASSERT_EQ(hooks->size(), 3);
  EXPECT_EQ((*hooks)[0], hook1);
  EXPECT_EQ((*hooks)[1], hook2);
  EXPECT_EQ((*hooks)[2], hook3);
}

TEST_F(HookManagerTest, AddHookAndAddHooksFiltersNullptrs) {
  auto& manager = HookManager::GetInstance();
  manager.AddHook(nullptr);
  EXPECT_TRUE(manager.GetHooks()->empty());

  std::shared_ptr<GeneralHook> valid_hook = std::make_shared<DummyTestHook1>();
  manager.AddHooks({nullptr, valid_hook, nullptr});

  auto hooks = manager.GetHooks();
  ASSERT_EQ(hooks->size(), 1);
  EXPECT_EQ((*hooks)[0], valid_hook);
}

TEST_F(HookManagerTest, ClearHooksRemovesAllHooks) {
  auto& manager = HookManager::GetInstance();
  std::shared_ptr<GeneralHook> hook1 = std::make_shared<DummyTestHook1>();
  std::shared_ptr<GeneralHook> hook2 = std::make_shared<DummyTestHook2>();
  manager.AddHooks({hook1, hook2});

  ASSERT_EQ(manager.GetHooks()->size(), 2);

  manager.ClearHooks();
  EXPECT_TRUE(manager.GetHooks()->empty());
}

TEST_F(HookManagerTest, CopyOnWriteSharesPointerOnReadsAndCopiesOnWrite) {
  auto& manager = HookManager::GetInstance();
  std::shared_ptr<GeneralHook> hook1 = std::make_shared<DummyTestHook1>();
  manager.AddHook(hook1);

  // Consecutive reads without writes return the exact same underlying vector.
  auto snapshot1 = manager.GetHooks();
  auto snapshot2 = manager.GetHooks();
  EXPECT_EQ(snapshot1.get(), snapshot2.get());
  ASSERT_EQ(snapshot1->size(), 1);

  // Writing creates a new vector while preserving the existing snapshot.
  std::shared_ptr<GeneralHook> hook2 = std::make_shared<DummyTestHook2>();
  manager.AddHook(hook2);

  auto snapshot3 = manager.GetHooks();
  EXPECT_NE(snapshot1.get(), snapshot3.get());
  EXPECT_EQ(snapshot1->size(), 1);
  EXPECT_EQ(snapshot3->size(), 2);
}

constexpr int kWriterSleepIntervalMs = 1;
constexpr int kReaderThreadCount = 8;
constexpr int kStressTestDurationMs = 100;

TEST_F(HookManagerTest, ThreadSafetyStressTest) {
  auto& manager = HookManager::GetInstance();
  std::atomic<bool> stop{false};

  // Writer thread adding hooks
  std::thread writer([&]() {
    while (!stop) {
      manager.AddHook(std::make_shared<DummyTestHook1>());
      std::this_thread::sleep_for(
          std::chrono::milliseconds(kWriterSleepIntervalMs));
    }
  });

  // Reader threads reading hooks
  std::vector<std::thread> readers;
  readers.reserve(kReaderThreadCount);
  for (int i = 0; i < kReaderThreadCount; ++i) {
    readers.emplace_back([&]() {
      while (!stop) {
        auto hooks = manager.GetHooks();
        volatile size_t hook_count = hooks->size();
        (void)hook_count;
      }
    });
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(kStressTestDurationMs));

  stop = true;
  writer.join();
  for (auto& reader_thread : readers) {
    reader_thread.join();
  }

  EXPECT_FALSE(manager.GetHooks()->empty());
}

}  // namespace
}  // namespace openfeature
