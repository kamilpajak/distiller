#ifndef THERMOMETER_H
#define THERMOMETER_H

#include <algorithm>
#include <memory> // Required for std::shared_ptr

#ifdef UNIT_TEST
#include <gmock/gmock.h>

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

#else
#include <DallasTemperature.h>
#include <OneWire.h>
#endif

/**
 * Class for managing a thermometer.
 */
class Thermometer {
private:
#ifdef UNIT_TEST
  std::shared_ptr<MockOneWire> oneWire;
  std::shared_ptr<MockDallasTemperature> sensors;
#else
  OneWire oneWire;           /**< OneWire object for communication. */
  DallasTemperature sensors; /**< DallasTemperature object for temperature sensing. */
#endif
  float readings[5];         /**< Array to store temperature readings. */
  int index;                 /**< Index for the current reading. */
  float lastMedian;          /**< Last calculated median temperature. */
  int readingsCount;         /**< Number of valid temperature readings stored. */

public:
#ifdef UNIT_TEST
  // Constructor for the test environment
  explicit Thermometer(std::shared_ptr<MockOneWire> ow, std::shared_ptr<MockDallasTemperature> ds) 
    : oneWire(ow), sensors(ds), index(0), lastMedian(0.0), readingsCount(0) {
    for (int i = 0; i < 5; i++) {
      readings[i] = 0.0;
    }
  }

  // Method to set lastMedian for testing purposes
  void setLastMedian(float median) {
    lastMedian = median;
  }
#else
  /**
   * Constructor for the Thermometer class.
   * @param pin The pin number for the thermometer sensor.
   */
  explicit Thermometer(int pin) : oneWire(pin), sensors(&oneWire), index(0), lastMedian(0.0), readingsCount(0) {
    sensors.begin();
    for (int i = 0; i < 5; i++) {
      readings[i] = 0.0;
    }
  }
#endif

  /**
   * Updates the temperature reading.
   */
  void updateTemperature() {
#ifdef UNIT_TEST
    sensors->requestTemperatures();
    // Update the last median before adding the new reading
    if (readingsCount == 5) {
      lastMedian = getTemperature();
    }
    readings[index] = sensors->getTempCByIndex(0);
#else
    sensors.requestTemperatures();
    // Update the last median before adding the new reading
    if (readingsCount == 5) {
      lastMedian = getTemperature();
    }
    readings[index] = sensors.getTempCByIndex(0);
#endif
    index = (index + 1) % 5;
    readingsCount = std::min(readingsCount + 1, 5); // Don't let readingsCount exceed 5
  }

  /**
   * Checks if there is a sudden temperature increase beyond a given threshold.
   * @param threshold The temperature increase threshold to check against.
   * @return True if a sudden temperature increase beyond the threshold is detected, false otherwise.
   */
  bool isSuddenTemperatureIncrease(float threshold) {
    if (readingsCount < 5) {
      return false; // Not enough readings to calculate the median
    }
    float currentMedian = getTemperature();
    float difference = currentMedian - lastMedian;
    float tolerance = 0.001; // Small tolerance for floating-point comparison
    return difference > threshold + tolerance;
  }

  /**
   * Returns the current temperature.
   * @return The current temperature in degrees Celsius.
   */
  float getTemperature() {
    float sortedReadings[5];
    std::copy(readings, readings + 5, sortedReadings);
    std::sort(sortedReadings, sortedReadings + 5);
    return sortedReadings[2]; // return the median
  }

  /**
   * Returns the last temperature reading.
   * @return The last temperature reading in degrees Celsius.
   */
  float getLastTemperature() {
    int lastIndex = (index - 1 + 5) % 5; // calculate the index of the last reading
    return readings[lastIndex];
  }
};

#endif // THERMOMETER_H
