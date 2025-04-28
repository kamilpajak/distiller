#ifndef TASK_MANAGER_IO_H
#define TASK_MANAGER_IO_H

#include <cstdint>

// Constants
constexpr uint32_t DEFAULT_TASK_RATE_MS = 1000;

// Define taskid_t for both test and non-test builds
using taskid_t = int;

#ifdef UNIT_TEST
#include <gmock/gmock.h>

// Define an abstract base class for TaskManager
class TaskManagerBase {
public:
  virtual ~TaskManagerBase() = default;
  virtual void runLoop() = 0;
  virtual void cancelTask(taskid_t taskId) = 0;
  virtual taskid_t scheduleFixedRate(uint32_t rate, void (*callback)()) = 0;
  virtual taskid_t scheduleFixedRate(uint32_t initialDelay, uint32_t rate, void (*callback)()) = 0;
};

// Create a mock implementation that uses Google Mock for the simple methods
// but custom implementation for the problematic ones
class TaskManager : public TaskManagerBase {
public:
  // Use Google Mock for simple methods
  MOCK_METHOD(void, runLoop, (), (override));
  MOCK_METHOD(void, cancelTask, (taskid_t taskId), (override));

  // Custom implementation for the problematic methods
  taskid_t scheduleFixedRate(uint32_t rate, void (*callback)()) override {
    // Call a mockable method that doesn't use function pointers
    return mockScheduleFixedRate2(rate);
  }

  taskid_t scheduleFixedRate(uint32_t initialDelay, uint32_t rate, void (*callback)()) override {
    // Call a mockable method that doesn't use function pointers
    return mockScheduleFixedRate3(initialDelay, rate);
  }

  // Mock methods that can be used for expectations
  MOCK_METHOD(taskid_t, mockScheduleFixedRate2, (uint32_t rate), ());
  MOCK_METHOD(taskid_t, mockScheduleFixedRate3, (uint32_t initialDelay, uint32_t rate), ());
};

extern TaskManager taskManager; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

#else
// Simple TaskManager implementation for non-test builds
class TaskManager {
public:
  void runLoop() {}
  void cancelTask(taskid_t taskId) {}

  static taskid_t scheduleFixedRate(uint32_t rate, void (*callback)()) {
    static taskid_t nextId = 1;
    return nextId++;
  }

  static taskid_t scheduleFixedRate(uint32_t initialDelay, uint32_t rate, void (*callback)()) {
    static taskid_t nextId = DEFAULT_TASK_RATE_MS;
    return nextId++;
  }
};

extern TaskManager taskManager; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
#endif

#endif // TASK_MANAGER_IO_H
