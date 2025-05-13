#include "test_constants.h"

#include <constants.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <vector>

// Define UNIT_TEST if not already defined
#ifndef UNIT_TEST
#define UNIT_TEST
#endif

// Include mock Arduino functions
#include "mock_arduino.h"
#include "test_mocks.h"

#include <hardware_interfaces.h>
#include <logger.h>
#include <scale.h>
#include <scale_controller.h>

// Mock implementations for scale interface
class MockScaleInterface : public IScaleInterface {
public:
  bool initialized = false;
  bool readyToRead = true;
  float weight = 0.0f;
  bool tared = false;
  int tareCount = 0;
  std::vector<float> readings;
  int dataPin = -1;
  int clockPin = -1;

  void begin() override { initialized = true; }

  bool is_ready() override { return readyToRead; }

  void set_scale(float /*scale*/) override {
    // Not used in our tests
  }

  void tare(uint8_t times = 10) override {
    tared = true;
    tareCount = times;
  }

  float get_units(uint8_t /*times*/ = 10) override {
    float value = weight;
    readings.push_back(value);
    return value;
  }

  void power_down() override {
    // Not used in our tests
  }

  void power_up() override {
    // Not used in our tests
  }

  void reset() {
    initialized = false;
    readyToRead = true;
    weight = 0.0f;
    tared = false;
    tareCount = 0;
    readings.clear();
  }

  // Helper to simulate disconnection/connection
  void simulateDisconnection() { readyToRead = false; }

  void simulateConnection() { readyToRead = true; }

  void setWeight(float newWeight) { weight = newWeight; }
};

// Using MockSerialInterface from test_mocks.h

// Helper function and MockSDInterface are defined in test_mocks.h

// Test fixture for Scale tests
class ScaleResilienceTest : public ::testing::Test {
protected:
  std::unique_ptr<MockSerialInterface> serialInterface;
  std::unique_ptr<MockSDInterface> sdInterface;
  std::unique_ptr<Logger> logger;
  std::unique_ptr<MockScaleInterface> scaleInterface;
  std::unique_ptr<Scale> scale;

  const int dataPin = 5;
  const int clockPin = 6;

  void SetUp() override {
    // Reset Arduino mock time
    setMillis(0);

    // Set up interfaces
    MockSerialInterface::reset();
    serialInterface = std::make_unique<MockSerialInterface>();
    sdInterface = std::make_unique<MockSDInterface>();
    logger = std::make_unique<Logger>(serialInterface.get());
    logger->begin(Logger::DEBUG_LEVEL);

    // Create and reset scale interface
    scaleInterface = std::make_unique<MockScaleInterface>();
    scaleInterface->reset();

    // Create scale with mocked interfaces
    scale = std::make_unique<Scale>(scaleInterface.get(), dataPin, clockPin, logger.get());
  }

  void TearDown() override {
    // Clean up will be handled by unique_ptr destructors
  }
};

/**
 * @brief Test case for Scale connection success.
 *
 * Given a Scale object with logger.
 * When the HX711 responds properly.
 * Then the scale should initialize successfully and be marked as connected.
 */
TEST_F(ScaleResilienceTest, ScaleConnectionSuccess) {
  // Scale interface ready to respond
  scaleInterface->simulateConnection();

  // Create a scale with our interfaces
  scale = std::make_unique<Scale>(scaleInterface.get(), dataPin, clockPin, logger.get());

  // Verify scale status
  EXPECT_TRUE(scale->isConnected());
  EXPECT_TRUE(scaleInterface->initialized);
  EXPECT_TRUE(scaleInterface->tared);

  // Verify logs
  EXPECT_TRUE(containsSubstring(MockSerialInterface::logs, "Initializing scale on pins"));
  EXPECT_TRUE(containsSubstring(MockSerialInterface::logs, "Scale connected successfully"));
  EXPECT_TRUE(containsSubstring(MockSerialInterface::logs, "Scale tared"));
}

/**
 * @brief Test case for Scale connection timeout.
 *
 * Given a Scale object with logger.
 * When the HX711 doesn't respond within the timeout.
 * Then the scale should fail initialization and be marked as disconnected.
 */
TEST_F(ScaleResilienceTest, ScaleConnectionTimeout) {
  // Scale interface not ready to respond
  scaleInterface->simulateDisconnection();

  // Create a scale with our interfaces
  scale = std::make_unique<Scale>(scaleInterface.get(), dataPin, clockPin, logger.get());

  // Time passes over the timeout
  advanceMillis(SCALE_CONNECTION_TIMEOUT_MS + 100);

  // Verify scale status
  EXPECT_FALSE(scale->isConnected());
  EXPECT_TRUE(scaleInterface->initialized);
  EXPECT_FALSE(scaleInterface->tared);

  // Verify logs
  EXPECT_TRUE(containsSubstring(MockSerialInterface::logs, "Initializing scale on pins"));
  EXPECT_TRUE(containsSubstring(MockSerialInterface::logs, "Scale connection timeout"));
}

/**
 * @brief Test case for Scale reading success.
 *
 * Given a connected Scale object.
 * When updating the weight.
 * Then the reading should succeed.
 */
TEST_F(ScaleResilienceTest, ScaleReadingSuccess) {
  const float testReading = 42.5f;

  // HX711 ready to respond with test reading
  scaleInterface->simulateConnection();
  scaleInterface->setWeight(testReading);

  // Create a scale with our interfaces
  scale = std::make_unique<Scale>(scaleInterface.get(), dataPin, clockPin, logger.get());

  // Update weight
  bool result = scale->updateWeight();

  // Verify result
  EXPECT_TRUE(result);
  EXPECT_FLOAT_EQ(testReading, scale->getLastWeight());

  // After initial setup logs
  MockSerialInterface::reset();

  // Update weight again to check debug log
  result = scale->updateWeight();

  // Verify logs
  EXPECT_TRUE(containsSubstring(MockSerialInterface::logs, "Scale reading: 42.50"));
}

/**
 * @brief Test case for Scale reading timeout.
 *
 * Given a connected Scale object.
 * When the HX711 doesn't respond during weight update.
 * Then the update should fail and the scale marked as disconnected.
 */
TEST_F(ScaleResilienceTest, ScaleReadingTimeout) {
  // Scale interface ready for setup but will fail during reading
  scaleInterface->simulateConnection();

  // Create a scale with our interfaces
  scale = std::make_unique<Scale>(scaleInterface.get(), dataPin, clockPin, logger.get());

  // Reset logger after setup
  MockSerialInterface::reset();

  // Now make scale interface unresponsive
  scaleInterface->simulateDisconnection();

  // Update weight
  bool result = scale->updateWeight();

  // Time passes over the timeout
  advanceMillis(SCALE_READ_TIMEOUT_MS + 100);

  // Verify result
  EXPECT_FALSE(result);
  EXPECT_FALSE(scale->isConnected()); // Should be marked as disconnected

  // Verify logs
  EXPECT_TRUE(containsSubstring(MockSerialInterface::logs, "Timeout waiting for scale data"));
}

/**
 * @brief Test case for Scale reconnection success.
 *
 * Given a disconnected Scale object.
 * When tryReconnect is called and HX711 responds.
 * Then the reconnection should succeed.
 */
TEST_F(ScaleResilienceTest, ScaleReconnectionSuccess) {
  // Scale interface not ready at first (connection will timeout)
  scaleInterface->simulateDisconnection();

  // Create a scale with our interfaces
  scale = std::make_unique<Scale>(scaleInterface.get(), dataPin, clockPin, logger.get());

  // Verify it's disconnected
  EXPECT_FALSE(scale->isConnected());

  // Reset logger
  MockSerialInterface::reset();

  // Now scale interface becomes responsive for reconnection
  scaleInterface->simulateConnection();

  // Try to reconnect
  bool reconnected = scale->tryReconnect();

  // Verify reconnection
  EXPECT_TRUE(reconnected);
  EXPECT_TRUE(scale->isConnected());

  // Verify logs
  EXPECT_TRUE(containsSubstring(MockSerialInterface::logs, "Attempting to reconnect scale"));
  EXPECT_TRUE(containsSubstring(MockSerialInterface::logs, "Scale reconnected successfully"));
}

/**
 * @brief Test case for Scale reconnection failure.
 *
 * Given a disconnected Scale object.
 * When tryReconnect is called and HX711 doesn't respond.
 * Then the reconnection should fail.
 */
TEST_F(ScaleResilienceTest, ScaleReconnectionFailure) {
  // Scale interface not ready (connection will timeout)
  scaleInterface->simulateDisconnection();

  // Create a scale with our interfaces
  scale = std::make_unique<Scale>(scaleInterface.get(), dataPin, clockPin, logger.get());

  // Verify it's disconnected
  EXPECT_FALSE(scale->isConnected());

  // Reset logger
  MockSerialInterface::reset();

  // Scale interface still not responsive
  scaleInterface->simulateDisconnection();

  // Try to reconnect
  bool reconnected = scale->tryReconnect();

  // Time passes over the timeout
  advanceMillis(SCALE_CONNECTION_TIMEOUT_MS + 100);

  // Verify reconnection failed
  EXPECT_FALSE(reconnected);
  EXPECT_FALSE(scale->isConnected());

  // Verify logs
  EXPECT_TRUE(containsSubstring(MockSerialInterface::logs, "Attempting to reconnect scale"));
  EXPECT_TRUE(containsSubstring(MockSerialInterface::logs, "Scale reconnection timeout"));
}

/**
 * @brief Test case for median weight calculation.
 *
 * Given a Scale object with multiple readings.
 * When getWeight is called.
 * Then it should return the median value of the readings.
 */
TEST_F(ScaleResilienceTest, MedianWeightCalculation) {
  // Scale interface ready to respond
  scaleInterface->simulateConnection();

  // Create a scale with our interfaces
  scale = std::make_unique<Scale>(scaleInterface.get(), dataPin, clockPin, logger.get());

  // Add a set of weight readings
  scaleInterface->setWeight(10.0f);
  scale->updateWeight();

  scaleInterface->setWeight(30.0f);
  scale->updateWeight();

  scaleInterface->setWeight(20.0f);
  scale->updateWeight();

  scaleInterface->setWeight(15.0f);
  scale->updateWeight();

  scaleInterface->setWeight(25.0f);
  scale->updateWeight();

  // The median should be 20.0f
  EXPECT_FLOAT_EQ(20.0f, scale->getWeight());
}