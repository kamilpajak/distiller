#include "../include/distillation_state_manager.h"

/**
 * Get the singleton instance of the DistillationStateManager.
 * @return Reference to the singleton instance.
 */
DistillationStateManager &DistillationStateManager::getInstance() {
  static DistillationStateManager instance; // Guaranteed to be destroyed.
  return instance;
}

/**
 * Set the current state of the distillation process.
 * Resets the start time for certain key states.
 * @param newState The new state to set.
 */
void DistillationStateManager::setState(DistillationState newState) {
  currentState = newState;
  if (newState == HEAT_UP || newState == STABILIZING || newState == FINALIZING) {
    startTime = millis();
  }
}

/**
 * Get the current state of the distillation process.
 * @return The current state.
 */
DistillationState DistillationStateManager::getState() const { return currentState; }

/**
 * Get the elapsed time since the last state change.
 * @return Elapsed time in milliseconds.
 */
unsigned long DistillationStateManager::getElapsedTime() const { return millis() - startTime; }

#ifdef UNIT_TEST
/**
 * Set the start time for testing purposes.
 * @param time The time to set in milliseconds.
 */
void DistillationStateManager::setStartTime(unsigned long time) { startTime = time; }

/**
 * Get the raw start time for testing purposes.
 * @return The start time in milliseconds.
 */
unsigned long DistillationStateManager::getStartTime() const { return startTime; }
#endif