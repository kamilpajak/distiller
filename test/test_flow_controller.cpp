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

// Define Arduino millis function
// This approach uses preprocessor definitions to replace Arduino functions with our mocks
#define millis ArduinoMock::millis

// Mock PID class
// This mocks the PID class from the PID_v1 library
class PID {
public:
  PID(double* input, double* output, double* setpoint, double kp, double ki, double kd, int controllerDirection) 
    : input(input), output(output), setpoint(setpoint) {}
  
  void SetMode(int mode) {}
  
  bool Compute() {
    // Simple mock implementation
    *output = *setpoint - *input;
    return true;
  }
  
private:
  double* input;
  double* output;
  double* setpoint;
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
private:
  MockValveController* valveController;
  MockScaleController* scaleController;
  double input, output, setpoint;
  PID pid;
  double flowRate;
  unsigned long startTime;
  double startVolume;

  double getCurrentVolume() {
    return scaleController->getWeight(MockDistillationStateManager::getInstance().getState()) / ALCOHOL_DENSITY;
  }

public:
  TestableFlowController(MockValveController* valveController, MockScaleController* scaleController)
    : valveController(valveController), scaleController(scaleController), input(0), output(0), setpoint(0),
      pid(&input, &output, &setpoint, 2, 5, 1, DIRECT), flowRate(0), startTime(0), startVolume(0) {
    pid.SetMode(AUTOMATIC);
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
    input = expectedVolume - (currentVolume - startVolume);
    pid.Compute();

    float tolerance = 0.1;
    if (output > tolerance) {
      valveController->openMainValve();
    } else if (output < -tolerance) {
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
  std::unique_ptr<TestableFlowController> flowController;
  MockDistillationStateManager& stateManager = MockDistillationStateManager::getInstance();

  void SetUp() override {
    ArduinoMock::setMillis(0);
    valveController = std::make_unique<MockValveController>();
    scaleController = std::make_unique<MockScaleController>();
    
    // Initial closeMainValve call in constructor
    EXPECT_CALL(*valveController, closeMainValve()).Times(1);
    
    flowController = std::make_unique<TestableFlowController>(valveController.get(), scaleController.get());
  }
};

TEST_F(FlowControllerTest, InitialFlowRateIsZero) {
  EXPECT_EQ(0, flowController->getFlowRate());
}

TEST_F(FlowControllerTest, SetFlowRateToZeroClosesMainValve) {
  // Arrange
  EXPECT_CALL(*valveController, closeMainValve()).Times(1);
  
  // Act
  flowController->setAndControlFlowRate(0);
  
  // Assert
  EXPECT_EQ(0, flowController->getFlowRate());
}

TEST_F(FlowControllerTest, SetFlowRateToPositiveValueOpensMainValveWhenBehindTarget) {
  // Arrange
  EXPECT_CALL(stateManager, getState())
    .WillRepeatedly(::testing::Return(HEARTS));
  
  // First call to getCurrentVolume in setAndControlFlowRate
  EXPECT_CALL(*scaleController, getWeight(HEARTS))
    .WillOnce(::testing::Return(0.0));
  
  // Second call to getCurrentVolume in setAndControlFlowRate
  EXPECT_CALL(*scaleController, getWeight(HEARTS))
    .WillOnce(::testing::Return(0.0));
  
  EXPECT_CALL(*valveController, openMainValve()).Times(1);
  
  // Act
  flowController->setAndControlFlowRate(10.0);
  
  // Assert
  EXPECT_EQ(10.0, flowController->getFlowRate());
}

TEST_F(FlowControllerTest, ClosesMainValveWhenAheadOfTarget) {
  // Arrange
  EXPECT_CALL(stateManager, getState())
    .WillRepeatedly(::testing::Return(HEARTS));
  
  // First call to getCurrentVolume in setAndControlFlowRate
  EXPECT_CALL(*scaleController, getWeight(HEARTS))
    .WillOnce(::testing::Return(0.0));
  
  // Set initial flow rate
  flowController->setAndControlFlowRate(10.0);
  
  // Advance time by 1 minute
  ArduinoMock::advanceMillis(60000);
  
  // Second call to getCurrentVolume in setAndControlFlowRate
  // Return a weight that corresponds to more volume than expected
  EXPECT_CALL(*scaleController, getWeight(HEARTS))
    .WillOnce(::testing::Return(20.0 * ALCOHOL_DENSITY)); // 20ml is more than 10ml/min * 1min
  
  EXPECT_CALL(*valveController, closeMainValve()).Times(1);
  
  // Act
  flowController->setAndControlFlowRate(10.0);
}

TEST_F(FlowControllerTest, MaintainsFlowRateWhenOnTarget) {
  // Arrange
  EXPECT_CALL(stateManager, getState())
    .WillRepeatedly(::testing::Return(HEARTS));
  
  // First call to getCurrentVolume in setAndControlFlowRate
  EXPECT_CALL(*scaleController, getWeight(HEARTS))
    .WillOnce(::testing::Return(0.0));
  
  // Set initial flow rate
  flowController->setAndControlFlowRate(10.0);
  
  // Advance time by 1 minute
  ArduinoMock::advanceMillis(60000);
  
  // Second call to getCurrentVolume in setAndControlFlowRate
  // Return a weight that corresponds to exactly the expected volume
  EXPECT_CALL(*scaleController, getWeight(HEARTS))
    .WillOnce(::testing::Return(10.0 * ALCOHOL_DENSITY)); // 10ml is exactly 10ml/min * 1min
  
  // No valve operations expected since we're on target
  
  // Act
  flowController->setAndControlFlowRate(10.0);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
