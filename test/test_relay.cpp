#include <gtest/gtest.h>
#include <gmock/gmock.h>

// Define UNIT_TEST if not already defined
#ifndef UNIT_TEST
#define UNIT_TEST
#endif

// Conditional includes based on test vs. production environment
#ifdef UNIT_TEST
// Test environment - use mock Arduino functions
namespace ArduinoMock {
  static testing::MockFunction<void(int, int)> mockPinMode;
  static testing::MockFunction<void(int, int)> mockDigitalWrite;
  
  void pinMode(int pin, int mode) {
    mockPinMode.Call(pin, mode);
  }
  
  void digitalWrite(int pin, int value) {
    mockDigitalWrite.Call(pin, value);
  }
  
  // Constants to match Arduino.h
  const int OUTPUT = 1;
  const int HIGH = 1;
  const int LOW = 0;
  
  void reset() {
    testing::Mock::VerifyAndClearExpectations(&mockPinMode);
    testing::Mock::VerifyAndClearExpectations(&mockDigitalWrite);
  }
}

// Include Arduino mock functions
// This approach uses preprocessor definitions to replace Arduino functions with our mocks
#define pinMode ArduinoMock::pinMode
#define digitalWrite ArduinoMock::digitalWrite
#define OUTPUT ArduinoMock::OUTPUT
#define HIGH ArduinoMock::HIGH
#define LOW ArduinoMock::LOW

// Include the Relay class
// In a production environment, we would modify the Relay class to use conditional compilation
#include "../src/relay.h"

// Undefine Arduino functions to avoid conflicts with other tests
#undef pinMode
#undef digitalWrite
#undef OUTPUT
#undef HIGH
#undef LOW

#else
// Production environment - would use real Arduino functions
// This section is not used in the test but shows how conditional compilation would work
#include <Arduino.h>
#endif

class RelayTest : public ::testing::Test {
protected:
  void SetUp() override {
    ArduinoMock::reset();
  }
  
  void TearDown() override {
    ArduinoMock::reset();
  }
};

/**
 * @brief Test case for InitializesCorrectly.
 * 
 * Given a pin number.
 * When a Relay object is initialized with the pin number.
 * Then pinMode should be called with OUTPUT and digitalWrite should be called with LOW.
 */
TEST_F(RelayTest, InitializesCorrectly) {
  // Arrange
  const int pin = 5;
  
  // Expect
  EXPECT_CALL(ArduinoMock::mockPinMode, Call(pin, ArduinoMock::OUTPUT));
  EXPECT_CALL(ArduinoMock::mockDigitalWrite, Call(pin, ArduinoMock::LOW));
  
  // Act
  Relay relay(pin);
}

/**
 * @brief Test case for TurnOnSetsOutputHigh.
 * 
 * Given a Relay object.
 * When turnOn is called.
 * Then digitalWrite should be called with HIGH.
 */
TEST_F(RelayTest, TurnOnSetsOutputHigh) {
  // Arrange
  const int pin = 5;
  
  // Setup expectations for constructor
  EXPECT_CALL(ArduinoMock::mockPinMode, Call(pin, ArduinoMock::OUTPUT));
  EXPECT_CALL(ArduinoMock::mockDigitalWrite, Call(pin, ArduinoMock::LOW));
  
  Relay relay(pin);
  
  // Expect
  EXPECT_CALL(ArduinoMock::mockDigitalWrite, Call(pin, ArduinoMock::HIGH));
  
  // Act
  relay.turnOn();
}

/**
 * @brief Test case for TurnOffSetsOutputLow.
 * 
 * Given a Relay object that is currently on.
 * When turnOff is called.
 * Then digitalWrite should be called with LOW.
 */
TEST_F(RelayTest, TurnOffSetsOutputLow) {
  // Arrange
  const int pin = 5;
  
  // Setup expectations for constructor
  EXPECT_CALL(ArduinoMock::mockPinMode, Call(pin, ArduinoMock::OUTPUT));
  EXPECT_CALL(ArduinoMock::mockDigitalWrite, Call(pin, ArduinoMock::LOW));
  
  Relay relay(pin);
  
  // Turn on first
  EXPECT_CALL(ArduinoMock::mockDigitalWrite, Call(pin, ArduinoMock::HIGH));
  relay.turnOn();
  
  // Expect
  EXPECT_CALL(ArduinoMock::mockDigitalWrite, Call(pin, ArduinoMock::LOW));
  
  // Act
  relay.turnOff();
}

/**
 * @brief Test case for TurnOnDoesNothingIfAlreadyOn.
 * 
 * Given a Relay object that is currently on.
 * When turnOn is called again.
 * Then no further calls to digitalWrite should occur.
 */
TEST_F(RelayTest, TurnOnDoesNothingIfAlreadyOn) {
  // Arrange
  const int pin = 5;
  
  // Setup expectations for constructor
  EXPECT_CALL(ArduinoMock::mockPinMode, Call(pin, ArduinoMock::OUTPUT));
  EXPECT_CALL(ArduinoMock::mockDigitalWrite, Call(pin, ArduinoMock::LOW));
  
  Relay relay(pin);
  
  // Turn on first
  EXPECT_CALL(ArduinoMock::mockDigitalWrite, Call(pin, ArduinoMock::HIGH));
  relay.turnOn();
  
  // Act & Assert - No more calls to digitalWrite expected
  relay.turnOn();
}

/**
 * @brief Test case for TurnOffDoesNothingIfAlreadyOff.
 * 
 * Given a Relay object that is currently off.
 * When turnOff is called again.
 * Then no further calls to digitalWrite should occur.
 */
TEST_F(RelayTest, TurnOffDoesNothingIfAlreadyOff) {
  // Arrange
  const int pin = 5;
  
  // Setup expectations for constructor
  EXPECT_CALL(ArduinoMock::mockPinMode, Call(pin, ArduinoMock::OUTPUT));
  EXPECT_CALL(ArduinoMock::mockDigitalWrite, Call(pin, ArduinoMock::LOW));
  
  Relay relay(pin);
  
  // Act & Assert - No more calls to digitalWrite expected
  relay.turnOff();
}
