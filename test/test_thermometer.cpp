#include "../src/constants.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>

// Define test-specific constants for temperature values
constexpr float TEST_TEMP_BASE = 20.0F;
constexpr float TEST_TEMP_SMALL_INCREMENT = 0.1F;
constexpr float TEST_TEMP_MEDIUM_INCREMENT = 1.0F;
constexpr float TEST_TEMP_LARGE_INCREMENT = 2.0F;
constexpr float TEST_TEMP_SUDDEN_INCREASE = 5.0F;

// Define UNIT_TEST if not already defined
#ifndef UNIT_TEST
#define UNIT_TEST
#endif

// Include the Thermometer class (now with conditional compilation)
#include "../src/thermometer.h"

class ThermometerTest : public ::testing::Test {
protected:
  std::shared_ptr<MockOneWire> oneWire;
  std::shared_ptr<MockDallasTemperature> sensors;
  std::unique_ptr<Thermometer> thermometer;

  void SetUp() override {
    oneWire = std::make_shared<MockOneWire>();
    sensors = std::make_shared<MockDallasTemperature>();
    thermometer = std::make_unique<Thermometer>(oneWire, sensors);
  }
};

/**
 * @brief Test case for GetTemperatureReturnsMedian.
 *
 * Given the thermometer has received 5 temperature readings.
 * When getTemperature is called.
 * Then it should return the median of the readings.
 */
TEST_F(ThermometerTest, GetTemperatureReturnsMedian) {
  // Arrange
  EXPECT_CALL(*sensors, requestTemperatures()).Times(READINGS_ARRAY_SIZE);

  EXPECT_CALL(*sensors, getTempCByIndex(0))
      .WillOnce(::testing::Return(TEST_TEMP_BASE))
      .WillOnce(::testing::Return(TEST_TEMP_BASE + TEST_TEMP_LARGE_INCREMENT))
      .WillOnce(::testing::Return(TEST_TEMP_BASE + TEST_TEMP_MEDIUM_INCREMENT))
      .WillOnce(::testing::Return(TEST_TEMP_BASE + TEST_TEMP_LARGE_INCREMENT + TEST_TEMP_MEDIUM_INCREMENT))
      .WillOnce(::testing::Return(TEST_TEMP_BASE - TEST_TEMP_MEDIUM_INCREMENT));

  // Act
  for (int i = 0; i < READINGS_ARRAY_SIZE; i++) {
    thermometer->updateTemperature();
  }

  // Assert
  EXPECT_FLOAT_EQ(TEST_TEMP_BASE + TEST_TEMP_MEDIUM_INCREMENT, thermometer->getTemperature());
}

/**
 * @brief Test case for DetectsSuddenTemperatureIncrease.
 *
 * Given the thermometer has received 5 stable readings followed by 5 readings with a sudden increase.
 * When isSuddenTemperatureIncrease is called after the initial stable readings.
 * Then it should return false.
 * When isSuddenTemperatureIncrease is called after the sudden increase readings.
 * Then it should return true.
 */
TEST_F(ThermometerTest, DetectsSuddenTemperatureIncrease) {
  // Arrange
  EXPECT_CALL(*sensors, requestTemperatures()).Times(10);

  // First 5 readings
  EXPECT_CALL(*sensors, getTempCByIndex(0))
      .WillOnce(::testing::Return(TEST_TEMP_BASE))
      .WillOnce(::testing::Return(TEST_TEMP_BASE + TEST_TEMP_SMALL_INCREMENT))
      .WillOnce(::testing::Return(TEST_TEMP_BASE + (TEST_TEMP_SMALL_INCREMENT * 2)))
      .WillOnce(::testing::Return(TEST_TEMP_BASE + TEST_TEMP_SMALL_INCREMENT))
      .WillOnce(::testing::Return(TEST_TEMP_BASE))
      // Next 5 readings with a sudden increase
      .WillOnce(::testing::Return(TEST_TEMP_BASE))
      .WillOnce(::testing::Return(TEST_TEMP_BASE + TEST_TEMP_SMALL_INCREMENT))
      .WillOnce(::testing::Return(TEST_TEMP_BASE + TEST_TEMP_SUDDEN_INCREASE)) // Sudden increase
      .WillOnce(::testing::Return(TEST_TEMP_BASE + TEST_TEMP_SUDDEN_INCREASE + TEST_TEMP_SMALL_INCREMENT))
      .WillOnce(::testing::Return(TEST_TEMP_BASE + TEST_TEMP_SUDDEN_INCREASE + (TEST_TEMP_SMALL_INCREMENT * 2)));

  // Act & Assert
  // Fill the buffer with initial readings
  for (int i = 0; i < READINGS_ARRAY_SIZE; i++) {
    thermometer->updateTemperature();
  }

  // Explicitly set lastMedian after filling the buffer
  thermometer->setLastMedian(thermometer->getTemperature());

  // No sudden increase yet (after filling the buffer)
  EXPECT_FALSE(thermometer->isSuddenTemperatureIncrease(TEMPERATURE_STABILIZATION_THRESHOLD_C));

  // Add readings with the sudden increase
  for (int i = 0; i < READINGS_ARRAY_SIZE; i++) {
    thermometer->updateTemperature();
  }

  // Now we should detect the sudden increase
  EXPECT_TRUE(thermometer->isSuddenTemperatureIncrease(TEMPERATURE_STABILIZATION_THRESHOLD_C));
}

/**
 * @brief Test case for GetLastTemperatureReturnsLastReading.
 *
 * Given the thermometer has received several temperature readings.
 * When getLastTemperature is called.
 * Then it should return the value of the most recent reading.
 */
TEST_F(ThermometerTest, GetLastTemperatureReturnsLastReading) {
  // Arrange
  EXPECT_CALL(*sensors, requestTemperatures()).Times(3);

  EXPECT_CALL(*sensors, getTempCByIndex(0))
      .WillOnce(::testing::Return(TEST_TEMP_BASE))
      .WillOnce(::testing::Return(TEST_TEMP_BASE + TEST_TEMP_MEDIUM_INCREMENT))
      .WillOnce(::testing::Return(TEST_TEMP_BASE + (TEST_TEMP_MEDIUM_INCREMENT * 2)));

  // Act
  thermometer->updateTemperature();
  thermometer->updateTemperature();
  thermometer->updateTemperature();

  // Assert
  EXPECT_FLOAT_EQ(TEST_TEMP_BASE + (TEST_TEMP_MEDIUM_INCREMENT * 2), thermometer->getLastTemperature());
}
