#ifndef THERMOMETER_H
#define THERMOMETER_H

#include <DallasTemperature.h>
#include <OneWire.h>
#include <algorithm>

/**
 * Class for managing a thermometer.
 */
class Thermometer {
private:
  OneWire oneWire;           /**< OneWire object for communication. */
  DallasTemperature sensors; /**< DallasTemperature object for temperature sensing. */
  float readings[5];         /**< Array to store temperature readings. */
  int index;                 /**< Index for the current reading. */
  float lastMedian;          /**< Last calculated median temperature. */
  int readingsCount;         /**< Number of valid temperature readings stored. */

public:
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

  /**
   * Updates the temperature reading.
   */
  void updateTemperature() {
    sensors.requestTemperatures();
    readings[index] = sensors.getTempCByIndex(0);
    index = (index + 1) % 5;
    readingsCount = std::min(readingsCount + 1, 5); // Don't let readingsCount exceed 5

    // Update the last median before calculating a new one
    if (readingsCount == 5) {
      lastMedian = getTemperature();
    }
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
    return currentMedian - lastMedian > threshold;
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
