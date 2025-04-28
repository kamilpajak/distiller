#include "../src/constants.h"
#include "test_constants.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>

// Define UNIT_TEST if not already defined
#ifndef UNIT_TEST
#define UNIT_TEST
#endif

// Include the mock Arduino functions
#include "mock_arduino.h"

// Include the distillation_state_manager.h file
#include "../src/distillation_state_manager.h"

class DistillationStateManagerTest : public ::testing::Test { // NOLINT(cppcoreguidelines-non-private-member-variables-in-classes)
protected:
  DistillationStateManager &stateManager = DistillationStateManager::getInstance();

  void SetUp() override {
    // Reset the mock Arduino time
    setMillis(0);
    
    // Reset the state manager to OFF state
    stateManager.setState(OFF);
  }
};

/**
 * @brief Test case for InitialStateIsOff.
 *
 * Given a new DistillationStateManager instance.
 * When the state is checked.
 * Then it should be OFF.
 */
TEST_F(DistillationStateManagerTest, InitialStateIsOff) { // NOLINT(cppcoreguidelines-owning-memory)
  // Assert
  EXPECT_EQ(OFF, stateManager.getState());
}

/**
 * @brief Test case for SetStateChangesState.
 *
 * Given a DistillationStateManager instance.
 * When setState is called with a new state.
 * Then the state should be updated.
 */
TEST_F(DistillationStateManagerTest, SetStateChangesState) { // NOLINT(cppcoreguidelines-owning-memory)
  // Act
  stateManager.setState(HEAT_UP);
  
  // Assert
  EXPECT_EQ(HEAT_UP, stateManager.getState());
  
  // Act again
  stateManager.setState(STABILIZING);
  
  // Assert again
  EXPECT_EQ(STABILIZING, stateManager.getState());
}

/**
 * @brief Test case for SetStateResetsTimerForSpecificStates.
 *
 * Given a DistillationStateManager instance.
 * When setState is called with HEAT_UP, STABILIZING, or FINALIZING.
 * Then the elapsed time should be reset.
 */
TEST_F(DistillationStateManagerTest, SetStateResetsTimerForSpecificStates) { // NOLINT(cppcoreguidelines-owning-memory)
  // Arrange - advance time
  advanceMillis(1000);
  
  // Act - set state to HEAT_UP
  stateManager.setState(HEAT_UP);
  
  // Assert - elapsed time should be 0
  EXPECT_EQ(0, stateManager.getElapsedTime());
  
  // Arrange - advance time again
  advanceMillis(1000);
  
  // Act - set state to STABILIZING
  stateManager.setState(STABILIZING);
  
  // Assert - elapsed time should be 0 again
  EXPECT_EQ(0, stateManager.getElapsedTime());
  
  // Arrange - advance time again
  advanceMillis(1000);
  
  // Act - set state to FINALIZING
  stateManager.setState(FINALIZING);
  
  // Assert - elapsed time should be 0 again
  EXPECT_EQ(0, stateManager.getElapsedTime());
}

/**
 * @brief Test case for SetStateDoesNotResetTimerForOtherStates.
 *
 * Given a DistillationStateManager instance.
 * When setState is called with states other than HEAT_UP, STABILIZING, or FINALIZING.
 * Then the elapsed time should not be reset.
 */
TEST_F(DistillationStateManagerTest, SetStateDoesNotResetTimerForOtherStates) { // NOLINT(cppcoreguidelines-owning-memory)
  // Reset to a known state
  setMillis(0);
  stateManager.setState(OFF);
  stateManager.setStartTime(0); // Explicitly set start time
  
  // Verify initial state
  EXPECT_EQ(OFF, stateManager.getState());
  EXPECT_EQ(0, stateManager.getElapsedTime());
  
  // Arrange - advance time
  setMillis(1000); // Use setMillis instead of advanceMillis for more control
  
  // Verify time advanced
  unsigned long currentTime = millis();
  EXPECT_EQ(1000, currentTime) << "Expected millis() to return 1000, but got " << currentTime;
  
  // Act - set state to EARLY_FORESHOTS
  stateManager.setState(EARLY_FORESHOTS);
  
  // Verify start time wasn't reset (should still be 0)
  EXPECT_EQ(0, stateManager.getStartTime()) << "Start time should not have been reset";
  
  // Assert - elapsed time should be 1000
  EXPECT_EQ(EARLY_FORESHOTS, stateManager.getState());
  unsigned long elapsedTime = stateManager.getElapsedTime();
  EXPECT_EQ(1000, elapsedTime) << "Expected elapsed time to be 1000, but got " << elapsedTime 
                              << ". Current millis() = " << millis() 
                              << ", startTime = " << stateManager.getStartTime();
  
  // Arrange - advance time again
  setMillis(2000); // Use setMillis instead of advanceMillis
  
  // Verify time advanced again
  currentTime = millis();
  EXPECT_EQ(2000, currentTime) << "Expected millis() to return 2000, but got " << currentTime;
  
  // Act - set state to LATE_FORESHOTS
  stateManager.setState(LATE_FORESHOTS);
  
  // Verify start time wasn't reset (should still be 0)
  EXPECT_EQ(0, stateManager.getStartTime()) << "Start time should not have been reset";
  
  // Assert - elapsed time should be 2000
  EXPECT_EQ(LATE_FORESHOTS, stateManager.getState());
  elapsedTime = stateManager.getElapsedTime();
  EXPECT_EQ(2000, elapsedTime) << "Expected elapsed time to be 2000, but got " << elapsedTime
                              << ". Current millis() = " << millis()
                              << ", startTime = " << stateManager.getStartTime();
}

/**
 * @brief Test case for GetElapsedTimeReturnsCorrectTime.
 *
 * Given a DistillationStateManager instance.
 * When time passes.
 * Then getElapsedTime should return the correct elapsed time.
 */
TEST_F(DistillationStateManagerTest, GetElapsedTimeReturnsCorrectTime) { // NOLINT(cppcoreguidelines-owning-memory)
  // Reset to a known state
  setMillis(0);
  stateManager.setState(OFF);
  stateManager.setStartTime(0); // Explicitly set start time
  
  // Verify initial state
  EXPECT_EQ(OFF, stateManager.getState());
  EXPECT_EQ(0, stateManager.getElapsedTime());
  
  // Arrange - advance time
  setMillis(1000); // Use setMillis instead of advanceMillis
  
  // Verify time advanced
  unsigned long currentTime = millis();
  EXPECT_EQ(1000, currentTime) << "Expected millis() to return 1000, but got " << currentTime;
  
  // Assert
  unsigned long elapsedTime = stateManager.getElapsedTime();
  EXPECT_EQ(1000, elapsedTime) << "Expected elapsed time to be 1000, but got " << elapsedTime
                              << ". Current millis() = " << millis()
                              << ", startTime = " << stateManager.getStartTime();
  
  // Arrange - advance time again
  setMillis(2000); // Use setMillis instead of advanceMillis
  
  // Verify time advanced again
  currentTime = millis();
  EXPECT_EQ(2000, currentTime) << "Expected millis() to return 2000, but got " << currentTime;
  
  // Assert again
  elapsedTime = stateManager.getElapsedTime();
  EXPECT_EQ(2000, elapsedTime) << "Expected elapsed time to be 2000, but got " << elapsedTime
                              << ". Current millis() = " << millis()
                              << ", startTime = " << stateManager.getStartTime();
}

/**
 * @brief Test case for FullDistillationProcessStateTransitions.
 *
 * Given a DistillationStateManager instance.
 * When transitioning through all states in a typical distillation process.
 * Then each state transition should work correctly.
 */
TEST_F(DistillationStateManagerTest, FullDistillationProcessStateTransitions) { // NOLINT(cppcoreguidelines-owning-memory)
  // Reset to a known state
  setMillis(0);
  
  // Start with OFF state
  stateManager.setState(OFF);
  stateManager.setStartTime(0); // Explicitly set start time
  EXPECT_EQ(OFF, stateManager.getState());
  EXPECT_EQ(0, stateManager.getElapsedTime());
  
  // Transition to HEAT_UP
  stateManager.setState(HEAT_UP);
  EXPECT_EQ(HEAT_UP, stateManager.getState());
  // Verify start time was reset
  EXPECT_EQ(0, stateManager.getStartTime());
  EXPECT_EQ(0, stateManager.getElapsedTime());
  
  // Simulate time passing during heating
  setMillis(test_time::ONE_MINUTE_MS);
  EXPECT_EQ(test_time::ONE_MINUTE_MS, stateManager.getElapsedTime());
  
  // Transition to STABILIZING
  stateManager.setState(STABILIZING);
  EXPECT_EQ(STABILIZING, stateManager.getState());
  // Verify start time was reset
  EXPECT_EQ(test_time::ONE_MINUTE_MS, stateManager.getStartTime());
  EXPECT_EQ(0, stateManager.getElapsedTime());
  
  // Simulate time passing during stabilization
  setMillis(test_time::ONE_MINUTE_MS * 2);
  EXPECT_EQ(test_time::ONE_MINUTE_MS, stateManager.getElapsedTime());
  
  // Transition to EARLY_FORESHOTS
  stateManager.setState(EARLY_FORESHOTS);
  EXPECT_EQ(EARLY_FORESHOTS, stateManager.getState());
  // Verify start time was NOT reset
  EXPECT_EQ(test_time::ONE_MINUTE_MS, stateManager.getStartTime());
  EXPECT_EQ(test_time::ONE_MINUTE_MS, stateManager.getElapsedTime());
  
  // Transition to LATE_FORESHOTS
  stateManager.setState(LATE_FORESHOTS);
  EXPECT_EQ(LATE_FORESHOTS, stateManager.getState());
  // Verify start time was NOT reset
  EXPECT_EQ(test_time::ONE_MINUTE_MS, stateManager.getStartTime());
  
  // Transition to HEADS
  stateManager.setState(HEADS);
  EXPECT_EQ(HEADS, stateManager.getState());
  // Verify start time was NOT reset
  EXPECT_EQ(test_time::ONE_MINUTE_MS, stateManager.getStartTime());
  
  // Transition to HEARTS
  stateManager.setState(HEARTS);
  EXPECT_EQ(HEARTS, stateManager.getState());
  // Verify start time was NOT reset
  EXPECT_EQ(test_time::ONE_MINUTE_MS, stateManager.getStartTime());
  
  // Transition to EARLY_TAILS
  stateManager.setState(EARLY_TAILS);
  EXPECT_EQ(EARLY_TAILS, stateManager.getState());
  // Verify start time was NOT reset
  EXPECT_EQ(test_time::ONE_MINUTE_MS, stateManager.getStartTime());
  
  // Transition to LATE_TAILS
  stateManager.setState(LATE_TAILS);
  EXPECT_EQ(LATE_TAILS, stateManager.getState());
  // Verify start time was NOT reset
  EXPECT_EQ(test_time::ONE_MINUTE_MS, stateManager.getStartTime());
  
  // Transition to FINALIZING
  stateManager.setState(FINALIZING);
  EXPECT_EQ(FINALIZING, stateManager.getState());
  // Verify start time WAS reset
  EXPECT_EQ(test_time::ONE_MINUTE_MS * 2, stateManager.getStartTime());
  EXPECT_EQ(0, stateManager.getElapsedTime());
  
  // Transition back to OFF
  stateManager.setState(OFF);
  EXPECT_EQ(OFF, stateManager.getState());
  // Verify start time was NOT reset
  EXPECT_EQ(test_time::ONE_MINUTE_MS * 2, stateManager.getStartTime());
}
