#ifndef THERMOMETER_H
#define THERMOMETER_H

#include "constants.h"

#include <algorithm>
#include <array>
#include <memory> // Required for std::shared_ptr
#if __cplusplus >= 202002L
#include <ranges>
#endif

#ifdef UNIT_TEST
#include <gmock/gmock.h>

// Mock classes for OneWire and DallasTemperature
class MockOneWire {
public:
  MOCK_METHOD(void, begin, (), ());
  MOCK_METHOD(uint8_t, reset, (), ());
  MOCK_METHOD(void, select, (const uint8_t *), ());
  MOCK_METHOD(void, write, (uint8_t), ());
  MOCK_METHOD(void, write_bytes, (const uint8_t *, uint16_t), ());
  MOCK_METHOD(uint8_t, read, (), ());
  MOCK_METHOD(void, read_bytes, (uint8_t *, uint16_t), ());
};

class MockDallasTemperature {
public:
  MOCK_METHOD(void, begin, (), ());
  MOCK_METHOD(void, requestTemperatures, (), ());
  MOCK_METHOD(float, getTempCByIndex, (uint8_t), ());
};

#elif defined(UNIT_TEST)
// For unit tests, we'll use the mocks defined above
#elif defined(NATIVE)
// For native builds, we'll provide a minimal implementation
class OneWire {
public:
  OneWire(int pin) {}
  void begin() {}
  uint8_t reset() { return 1; }
  void select(const uint8_t *addr) {}
  void write(uint8_t v) {}
  void write_bytes(const uint8_t *buf, uint16_t count) {}
  uint8_t read() { return 0; }
  void read_bytes(uint8_t *buf, uint16_t count) {}
};

class DallasTemperature {
public:
  DallasTemperature(OneWire *wire) {}
  void begin() {}
  void requestTemperatures() {}
  float getTempCByIndex(uint8_t index) { return 20.0f; }
};
#else
// Use angle brackets for library includes - for production build
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
  std::array<float, READINGS_ARRAY_SIZE> readings{}; /**< Array to store temperature readings. */
  int index{0};                                      /**< Index for the current reading. */
  float lastMedian{0.0F};                            /**< Last calculated median temperature. */
  int readingsCount{0};                              /**< Number of valid temperature readings stored. */

public:
#ifdef UNIT_TEST
  // Constructor for the test environment
  explicit Thermometer(std::shared_ptr<MockOneWire> ow, std::shared_ptr<MockDallasTemperature> ds)
    : oneWire(std::move(ow)), sensors(std::move(ds)) {
    for (float &reading : readings) {
      reading = 0.0F;
    }
  }

  // Method to set lastMedian for testing purposes
  void setLastMedian(float median) { lastMedian = median; }
#else
  /**
   * Constructor for the Thermometer class.
   * @param pin The pin number for the thermometer sensor.
   */
  explicit Thermometer(int pin) : oneWire(pin), sensors(&oneWire) {
    sensors.begin();
    for (float &reading : readings) {
      reading = 0.0F;
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
    if (readingsCount == READINGS_ARRAY_SIZE) {
      lastMedian = getTemperature();
    }
    readings[index] = sensors->getTempCByIndex(0); // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
#else
    sensors.requestTemperatures();
    // Update the last median before adding the new reading
    if (readingsCount == READINGS_ARRAY_SIZE) {
      lastMedian = getTemperature();
    }
    readings[index] = sensors.getTempCByIndex(0); // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
#endif
    index = (index + 1) % READINGS_ARRAY_SIZE;
    readingsCount =
        std::min(readingsCount + 1, READINGS_ARRAY_SIZE); // Don't let readingsCount exceed READINGS_ARRAY_SIZE
  }

  /**
   * Checks if there is a sudden temperature increase beyond a given threshold.
   * @param threshold The temperature increase threshold to check against.
   * @return True if a sudden temperature increase beyond the threshold is detected, false otherwise.
   */
  [[nodiscard]] bool isSuddenTemperatureIncrease(float threshold) const {
    if (readingsCount < READINGS_ARRAY_SIZE) {
      return false; // Not enough readings to calculate the median
    }
    float currentMedian = getTemperature();
    float difference = currentMedian - lastMedian;
    return difference > threshold + TEMPERATURE_COMPARISON_TOLERANCE;
  }

  /**
   * Returns the current temperature.
   * @return The current temperature in degrees Celsius.
   */
  [[nodiscard]] float getTemperature() const {
    std::array<float, READINGS_ARRAY_SIZE> sortedReadings{};
#if __cplusplus >= 202002L
    std::ranges::copy(readings, sortedReadings.begin());
    std::ranges::sort(sortedReadings);
#else
    std::copy(readings.begin(), readings.end(), sortedReadings.begin()); // NOLINT(modernize-use-ranges)
    std::sort(sortedReadings.begin(), sortedReadings.end());             // NOLINT(modernize-use-ranges)
#endif
    return sortedReadings[READINGS_ARRAY_MIDDLE_INDEX]; // return the median
  }

  /**
   * Returns the last temperature reading.
   * @return The last temperature reading in degrees Celsius.
   */
  [[nodiscard]] float getLastTemperature() const {
    int lastIndex = (index - 1 + READINGS_ARRAY_SIZE) % READINGS_ARRAY_SIZE; // calculate the index of the last reading
    return readings[lastIndex]; // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
  }
};

#endif // THERMOMETER_H
