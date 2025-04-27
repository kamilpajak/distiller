#include <gmock/gmock.h>
#include <gtest/gtest.h>

// Define UNIT_TEST if not already defined
#ifndef UNIT_TEST
#define UNIT_TEST
#endif

// Include the mock Arduino functions
#include "mock_arduino.h"

// Include the Relay class
#include "../src/relay.h"

#ifdef UNIT_TEST
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
  void SetUp() override { ArduinoMockFixture::reset(); }

  void TearDown() override { ArduinoMockFixture::reset(); }
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
  EXPECT_CALL(ArduinoMockFixture::mockPinMode(), Call(pin, OUTPUT));
  EXPECT_CALL(ArduinoMockFixture::mockDigitalWrite(), Call(pin, LOW));

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
  EXPECT_CALL(ArduinoMockFixture::mockPinMode(), Call(pin, OUTPUT));
  EXPECT_CALL(ArduinoMockFixture::mockDigitalWrite(), Call(pin, LOW));

  Relay relay(pin);

  // Expect
  EXPECT_CALL(ArduinoMockFixture::mockDigitalWrite(), Call(pin, HIGH));

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
  EXPECT_CALL(ArduinoMockFixture::mockPinMode(), Call(pin, OUTPUT));
  EXPECT_CALL(ArduinoMockFixture::mockDigitalWrite(), Call(pin, LOW));

  Relay relay(pin);

  // Turn on first
  EXPECT_CALL(ArduinoMockFixture::mockDigitalWrite(), Call(pin, HIGH));
  relay.turnOn();

  // Expect
  EXPECT_CALL(ArduinoMockFixture::mockDigitalWrite(), Call(pin, LOW));

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
  EXPECT_CALL(ArduinoMockFixture::mockPinMode(), Call(pin, OUTPUT));
  EXPECT_CALL(ArduinoMockFixture::mockDigitalWrite(), Call(pin, LOW));

  Relay relay(pin);

  // Turn on first
  EXPECT_CALL(ArduinoMockFixture::mockDigitalWrite(), Call(pin, HIGH));
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
  EXPECT_CALL(ArduinoMockFixture::mockPinMode(), Call(pin, OUTPUT));
  EXPECT_CALL(ArduinoMockFixture::mockDigitalWrite(), Call(pin, LOW));

  Relay relay(pin);

  // Act & Assert - No more calls to digitalWrite expected
  relay.turnOff();
}
