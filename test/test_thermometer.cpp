#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>

// Define UNIT_TEST if not already defined
#ifndef UNIT_TEST
#define UNIT_TEST
#endif

// Conditional includes based on test vs. production environment
#ifdef UNIT_TEST
// Test environment - use mock classes
// Mock classes for OneWire and DallasTemperature
class MockOneWire {
public:
  MOCK_METHOD(void, begin, (), ());
  MOCK_METHOD(uint8_t, reset, (), ());
  MOCK_METHOD(void, select, (const uint8_t*), ());
  MOCK_METHOD(void, write, (uint8_t), ());
  MOCK_METHOD(void, write_bytes, (const uint8_t*, uint16_t), ());
  MOCK_METHOD(uint8_t, read, (), ());
  MOCK_METHOD(void, read_bytes, (uint8_t*, uint16_t), ());
};

class MockDallasTemperature {
public:
  MOCK_METHOD(void, begin, (), ());
  MOCK_METHOD(void, requestTemperatures, (), ());
  MOCK_METHOD(float, getTempCByIndex, (uint8_t), ());
};

// Modified Thermometer class for testing
// This is a testable version of the Thermometer class from src/thermometer.h
// In a production environment, we would use conditional compilation in the original class
class TestableThermometer {
private:
  std::shared_ptr<MockOneWire> oneWire;
  std::shared_ptr<MockDallasTemperature> sensors;
  float readings[5];
  int index;
  float lastMedian;
  int readingsCount;

public:
  explicit TestableThermometer(std::shared_ptr<MockOneWire> ow, std::shared_ptr<MockDallasTemperature> ds) 
    : oneWire(ow), sensors(ds), index(0), lastMedian(0.0), readingsCount(0) {
    for (int i = 0; i < 5; i++) {
      readings[i] = 0.0;
    }
  }

  void updateTemperature() {
    sensors->requestTemperatures();
    readings[index] = sensors->getTempCByIndex(0);
    index = (index + 1) % 5;
    readingsCount = std::min(readingsCount + 1, 5);

    if (readingsCount == 5) {
      lastMedian = getTemperature();
    }
  }

  bool isSuddenTemperatureIncrease(float threshold) {
    if (readingsCount < 5) {
      return false;
    }
    float currentMedian = getTemperature();
    return currentMedian - lastMedian > threshold;
  }

  float getTemperature() {
    float sortedReadings[5];
    std::copy(readings, readings + 5, sortedReadings);
    std::sort(sortedReadings, sortedReadings + 5);
    return sortedReadings[2]; // return the median
  }

  float getLastTemperature() {
    int lastIndex = (index - 1 + 5) % 5;
    return readings[lastIndex];
  }
};

class ThermometerTest : public ::testing::Test {
protected:
  std::shared_ptr<MockOneWire> oneWire;
  std::shared_ptr<MockDallasTemperature> sensors;
  std::unique_ptr<TestableThermometer> thermometer;

  void SetUp() override {
    oneWire = std::make_shared<MockOneWire>();
    sensors = std::make_shared<MockDallasTemperature>();
    thermometer = std::make_unique<TestableThermometer>(oneWire, sensors);
  }
};

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
  
  // No sudden increase yet
  EXPECT_FALSE(thermometer->isSuddenTemperatureIncrease(3.0));
  
  // Add readings with the sudden increase
  for (int i = 0; i < 5; i++) {
    thermometer->updateTemperature();
  }
  
  // Now we should detect the sudden increase
  EXPECT_TRUE(thermometer->isSuddenTemperatureIncrease(3.0));
}

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

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
