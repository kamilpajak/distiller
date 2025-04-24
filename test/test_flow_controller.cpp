#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>

// Define UNIT_TEST if not already defined
#ifndef UNIT_TEST
#define UNIT_TEST
#endif

// Include the distillation_state_manager.h file to get the DistillationState enum
#include "../src/distillation_state_manager.h"

// Conditional includes based on test vs. production environment
#ifdef UNIT_TEST
// Test environment - use mock classes and functions

// Mock Arduino functions
namespace ArduinoMock {
  static unsigned long currentMillis = 0;
  
  unsigned long millis() {
    return currentMillis;
  }
  
  void setMillis(unsigned long newMillis) {
    currentMillis = newMillis;
  }
  
  void advanceMillis(unsigned long millisToAdvance) {
    currentMillis += millisToAdvance;
  }
}


// Mock PID class
// This mocks the PID class from the PID_v1 library
class MockPID {
public:
  MOCK_METHOD(void, SetMode, (int), ());
  MOCK_METHOD(bool, Compute, (), ());
  
  // Pointers to the input, output, and setpoint variables
  double* input;
  double* output;
  double* setpoint;

  // Constructor to accept pointers to the variables
  MockPID(double* input, double* output, double* setpoint, double kp, double ki, double kd, int controllerDirection)
    : input(input), output(output), setpoint(setpoint) {}
};

// Define PID constants to match the PID_v1 library
#define AUTOMATIC 1
#define DIRECT 0

// Mock ValveController
// This mocks the ValveController class from src/valve_controller.h
class MockValveController {
public:
  MOCK_METHOD(void, openMainValve, (), ());
  MOCK_METHOD(void, closeMainValve, (), ());
};

// DistillationState enum is now included from distillation_state_manager.h

// Define constants
const double ALCOHOL_DENSITY = 0.868;

// Mock ScaleController
class MockScaleController {
public:
  MOCK_METHOD(double, getWeight, (DistillationState), (const));
};

// Mock DistillationStateManager
class MockDistillationStateManager {
public:
  MOCK_METHOD(DistillationState, getState, (), (const));
  
  static MockDistillationStateManager& getInstance() {
    static MockDistillationStateManager instance;
    return instance;
  }
};

// Testable FlowController
// This is a testable version of the FlowController class from src/flow_controller.h
// In a production environment, we would use conditional compilation in the original class
class TestableFlowController {
public: // Changed from private to public for testing
  MockValveController* valveController;
  MockScaleController* scaleController;
  MockPID* pid; // Use a pointer to the mock PID
  double flowRate;
  unsigned long startTime;
  double startVolume;

private: // Keep other members private

  double getCurrentVolume() {
    return scaleController->getWeight(MockDistillationStateManager::getInstance().getState()) / ALCOHOL_DENSITY;
  }

public:
  // Accept a pointer to the mock PID in the constructor
  TestableFlowController(MockValveController* valveController, MockScaleController* scaleController, MockPID* pid)
    : valveController(valveController), scaleController(scaleController), pid(pid), flowRate(0), startTime(0), startVolume(0) {
    pid->SetMode(AUTOMATIC);
    valveController->closeMainValve();
  }

  double getFlowRate() { return flowRate; }

  void setAndControlFlowRate(double newFlowRate) {
    const double epsilon = 0.001;

    if (abs(newFlowRate - flowRate) > epsilon) {
      flowRate = newFlowRate;
      startVolume = getCurrentVolume();
      startTime = millis();
    }

    if (flowRate == 0) {
      valveController->closeMainValve();
      return;
    }

    unsigned long currentMillis = millis();
    float elapsedTimeInMinutes = (currentMillis - startTime) / 60000.0;
    float expectedVolume = flowRate * elapsedTimeInMinutes;
    float currentVolume = getCurrentVolume();
    *pid->input = expectedVolume - (currentVolume - startVolume); // Access input through pid pointer
    pid->Compute(); // Use arrow operator

    float tolerance = 0.1;
    if (*pid->output > tolerance) { // Access output through pid pointer
      valveController->openMainValve();
    } else if (*pid->output < -tolerance) { // Access output through pid pointer
      valveController->closeMainValve();
    }
  }
};

#else
// Production environment - would use real Arduino functions and libraries
// This section is not used in the test but shows how conditional compilation would work
#include <Arduino.h>
#include "PID_v1.h"
#include "constants.h"
#include "scale_controller.h"
#include "valve_controller.h"
#include "distillation_state_manager.h"
#endif

class FlowControllerTest : public ::testing::Test {
protected:
  std::unique_ptr<MockValveController> valveController;
  std::unique_ptr<MockScaleController> scaleController;
  std::unique_ptr<MockPID> pid; // Use a unique_ptr for the mock PID
  std::unique_ptr<TestableFlowController> flowController;
  MockDistillationStateManager& stateManager = MockDistillationStateManager::getInstance();

  // Move input, output, and setpoint to the test fixture
  double input, output, setpoint;

  void SetUp() override {
    ArduinoMock::setMillis(0);
    valveController = std::make_unique<MockValveController>();
    scaleController = std::make_unique<MockScaleController>();
    
    // Initialize input, output, and setpoint
    input = 0;
    output = 0;
    setpoint = 0;

    // Create the mock PID object using pointers to the test fixture's members
    pid = std::make_unique<MockPID>(&input, &output, &setpoint, 2, 5, 1, DIRECT);
    
    // Create the flow controller and pass the mock PID
    flowController = std::make_unique<TestableFlowController>(valveController.get(), scaleController.get(), pid.get());

    // Initial closeMainValve call in constructor and SetMode call
    EXPECT_CALL(*valveController, closeMainValve()).Times(1);
    EXPECT_CALL(*pid, SetMode(AUTOMATIC)).Times(1);

    // Create the flow controller and pass the mock PID
    flowController = std::make_unique<TestableFlowController>(valveController.get(), scaleController.get(), pid.get());
  }
};

/**
 * @brief Test case for InitialFlowRateIsZero.
 * 
 * Given a new FlowController object is created.
 * When the flow rate is checked.
 * Then it should be zero.
 */
TEST_F(FlowControllerTest, InitialFlowRateIsZero) {
  EXPECT_EQ(0, flowController->getFlowRate());
}

/**
 * @brief Test case for SetFlowRateToZeroClosesMainValve.
 * 
 * Given a FlowController object with a non-zero flow rate.
 * When the flow rate is set to zero.
 * Then the main valve should be closed.
 */
TEST_F(FlowControllerTest, SetFlowRateToZeroClosesMainValve) {
  // Arrange
  EXPECT_CALL(*valveController, closeMainValve()).Times(1);
  
  // Act
  flowController->setAndControlFlowRate(0);
  
  // Assert
  EXPECT_EQ(0, flowController->getFlowRate());
}

/**
 * @brief Test case for SetFlowRateToPositiveValueOpensMainValveWhenBehindTarget.
 * 
 * Given a FlowController object with a zero flow rate and the current volume is behind the expected volume.
 * When the flow rate is set to a positive value.
 * Then the main valve should be opened.
 */
TEST_F(FlowControllerTest, SetFlowRateToPositiveValueOpensMainValveWhenBehindTarget) {
  // Arrange
  EXPECT_CALL(stateManager, getState())
    .WillRepeatedly(::testing::Return(HEARTS));
  
  // Expect two calls to getWeight in setAndControlFlowRate
  EXPECT_CALL(*scaleController, getWeight(HEARTS))
    .Times(2)
    .WillRepeatedly(::testing::Return(0.0)); // Simulate no weight change
  
  // Expect Compute to be called and set output to a positive value
  EXPECT_CALL(*pid, Compute())
    .WillOnce(::testing::DoAll(::testing::Assign(&output, 1.0), ::testing::Return(true))); // Simulate PID output > tolerance
  
  // Expect openMainValve to be called once
  EXPECT_CALL(*valveController, openMainValve()).Times(1);
  
  // Act
  flowController->setAndControlFlowRate(10.0);
  
  // Assert
  EXPECT_EQ(10.0, flowController->getFlowRate());
}

/**
 * @brief Test case for ClosesMainValveWhenAheadOfTarget.
 * 
 * Given a FlowController object with a positive flow rate and the current volume is ahead of the expected volume.
 * When setAndControlFlowRate is called.
 * Then the main valve should be closed.
 */
TEST_F(FlowControllerTest, ClosesMainValveWhenAheadOfTarget) {
  // Arrange
  EXPECT_CALL(stateManager, getState())
    .WillRepeatedly(::testing::Return(HEARTS));
  
  // Expect calls to getWeight
  EXPECT_CALL(*scaleController, getWeight(HEARTS))
    .WillOnce(::testing::Return(0.0)) // First call in initial setAndControlFlowRate
    .WillOnce(::testing::Return(20.0 * ALCOHOL_DENSITY)); // Second call after advancing time
  
  // Set initial flow rate
  flowController->setAndControlFlowRate(10.0);
  
  // Advance time by 1 minute
  ArduinoMock::advanceMillis(60000);
  
  // Expect Compute to be called and set output to a negative value
  EXPECT_CALL(*pid, Compute())
    .WillOnce(::testing::DoAll(::testing::Assign(&output, -1.0), ::testing::Return(true))); // Simulate PID output < -tolerance
  
  // Expect closeMainValve to be called once
  EXPECT_CALL(*valveController, closeMainValve()).Times(1);
  
  // Act
  flowController->setAndControlFlowRate(10.0);
}

/**
 * @brief Test case for MaintainsFlowRateWhenOnTarget.
 * 
 * Given a FlowController object with a positive flow rate and the current volume is on target with the expected volume.
 * When setAndControlFlowRate is called.
 * Then no valve operations should occur.
 */
TEST_F(FlowControllerTest, MaintainsFlowRateWhenOnTarget) {
  // Arrange
  EXPECT_CALL(stateManager, getState())
    .WillRepeatedly(::testing::Return(HEARTS));
  
  // Expect calls to getWeight
  EXPECT_CALL(*scaleController, getWeight(HEARTS))
    .WillOnce(::testing::Return(0.0)) // First call in initial setAndControlFlowRate
    .WillOnce(::testing::Return(10.0 * ALCOHOL_DENSITY)); // Second call after advancing time
  
  // Set initial flow rate
  flowController->setAndControlFlowRate(10.0);
  
  // Advance time by 1 minute
  ArduinoMock::advanceMillis(60000);
  
  // Expect Compute to be called and set output to a value within tolerance
  EXPECT_CALL(*pid, Compute())
    .WillOnce(::testing::DoAll(::testing::Assign(&output, 0.05), ::testing::Return(true))); // Simulate PID output within tolerance
  
  // No valve operations expected since we're on target
  EXPECT_CALL(*valveController, openMainValve()).Times(0);
  EXPECT_CALL(*valveController, closeMainValve()).Times(0);
  
  // Act
  flowController->setAndControlFlowRate(10.0);
}
