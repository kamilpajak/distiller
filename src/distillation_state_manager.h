#ifndef DISTILLATION_STATE_MANAGER_H
#define DISTILLATION_STATE_MANAGER_H

#include <Arduino.h>

/**
 * Enum for distillation states.
 */
enum DistillationState { OFF, HEAT_UP, FORESHOTS, HEADS, MIDDLINGS, HEARTS, TAILS };

class DistillationStateManager {
private:
  DistillationState currentState;
  unsigned long startTime;

  // Private constructor to prevent instancing.
  DistillationStateManager() : currentState(OFF), startTime(millis()) {}

public:
  // Deleted copy constructor and assignment operator to prevent copying.
  DistillationStateManager(const DistillationStateManager &) = delete;
  DistillationStateManager &operator=(const DistillationStateManager &) = delete;

  // Static method to control access to the singleton instance.
  static DistillationStateManager &getInstance() {
    static DistillationStateManager instance; // Guaranteed to be destroyed.
    return instance;
  }

  void setState(DistillationState newState) {
    currentState = newState;
    if (newState == HEAT_UP) {
      startTime = millis();
    }
  }

  DistillationState getState() const { return currentState; }

  unsigned long getElapsedTime() const { return millis() - startTime; }
};

#endif // DISTILLATION_STATE_MANAGER_H
