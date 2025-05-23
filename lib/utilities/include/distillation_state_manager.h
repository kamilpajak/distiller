#ifndef DISTILLATION_STATE_MANAGER_H
#define DISTILLATION_STATE_MANAGER_H

#include <cstdint> // For std::uint8_t

#ifndef UNIT_TEST
#include "Arduino.h"
#else
// Provide a mock or placeholder for millis() in the test environment
// The actual mock implementation will be in the test files
#include "mock_arduino.h"
#endif

/**
 * Enum for distillation states.
 */
enum DistillationState : std::uint8_t {
  OFF,
  HEAT_UP,
  STABILIZING,
  EARLY_FORESHOTS,
  LATE_FORESHOTS,
  HEADS,
  HEARTS,
  EARLY_TAILS,
  LATE_TAILS,
  FINALIZING
};

class DistillationStateManager {
private:
  DistillationState currentState{OFF};
  unsigned long startTime{millis()};

  // Private constructor to prevent instancing.
  DistillationStateManager() = default;

public:
  // Rule of five: deleted copy/move constructors and assignment operators to prevent copying/moving.
  DistillationStateManager(const DistillationStateManager &) = delete;
  DistillationStateManager &operator=(const DistillationStateManager &) = delete;
  DistillationStateManager(DistillationStateManager &&) = delete;
  DistillationStateManager &operator=(DistillationStateManager &&) = delete;

  // Virtual destructor for proper cleanup in case of inheritance
  virtual ~DistillationStateManager() = default;

  // Static method to control access to the singleton instance.
  static DistillationStateManager &getInstance();

  void setState(DistillationState newState);

  [[nodiscard]] DistillationState getState() const;

  [[nodiscard]] unsigned long getElapsedTime() const;

// For testing purposes only
#ifdef UNIT_TEST
  void setStartTime(unsigned long time);
  unsigned long getStartTime() const;
#endif
};

#endif // DISTILLATION_STATE_MANAGER_H
