#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>

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
  EXPECT_CALL(*sensors, requestTemperatures())
    .Times(5);
  
  EXPECT_CALL(*sensors, getTempCByIndex(0))
    .WillOnce(::testing::Return(20.0))
    .WillOnce(::testing::Return(22.0))
    .WillOnce(::testing::Return(21.0))
    .WillOnce(::testing::Return(23.0))
    .WillOnce(::testing::Return(19.0));

  // Act
  for (int i = 0; i < 5; i++) {
    thermometer->updateTemperature();
  }

  // Assert
  EXPECT_FLOAT_EQ(21.0, thermometer->getTemperature());
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
  EXPECT_CALL(*sensors, requestTemperatures())
    .Times(10);
  
  // First 5 readings
  EXPECT_CALL(*sensors, getTempCByIndex(0))
    .WillOnce(::testing::Return(20.0))
    .WillOnce(::testing::Return(20.1))
    .WillOnce(::testing::Return(20.2))
    .WillOnce(::testing::Return(20.1))
    .WillOnce(::testing::Return(20.0))
    // Next 5 readings with a sudden increase
    .WillOnce(::testing::Return(20.0))
    .WillOnce(::testing::Return(20.1))
    .WillOnce(::testing::Return(25.0)) // Sudden increase
    .WillOnce(::testing::Return(25.1))
    .WillOnce(::testing::Return(25.2));

  // Act & Assert
  // Fill the buffer with initial readings
  for (int i = 0; i < 5; i++) {
    thermometer->updateTemperature();
  }

  // Explicitly set lastMedian after filling the buffer
  thermometer->setLastMedian(thermometer->getTemperature());

  // No sudden increase yet (after filling the buffer)
  EXPECT_FALSE(thermometer->isSuddenTemperatureIncrease(3.0));

  // Add readings with the sudden increase
  for (int i = 0; i < 5; i++) {
    thermometer->updateTemperature();
  }
  
  // Now we should detect the sudden increase
  EXPECT_TRUE(thermometer->isSuddenTemperatureIncrease(3.0));
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
  EXPECT_CALL(*sensors, requestTemperatures())
    .Times(3);
  
  EXPECT_CALL(*sensors, getTempCByIndex(0))
    .WillOnce(::testing::Return(20.0))
    .WillOnce(::testing::Return(21.0))
    .WillOnce(::testing::Return(22.0));

  // Act
  thermometer->updateTemperature();
  thermometer->updateTemperature();
  thermometer->updateTemperature();

  // Assert
  EXPECT_FLOAT_EQ(22.0, thermometer->getLastTemperature());
}
