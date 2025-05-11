#ifndef SCALE_H
#define SCALE_H

#ifndef UNIT_TEST
#include "../include/HX711.h"
#include "logger.h"
#endif
#include "constants.h"

#include <algorithm>
#include <array>

/**
 * Class for managing a scale.
 */
class Scale {
private:
  HX711 scale;                                       /**< HX711 object for weight measurement. */
  std::array<float, READINGS_ARRAY_SIZE> readings{}; /**< Array to store weight readings. */
  int index{0};                                      /**< Index for the current reading. */
  bool connected{false};                            /**< Whether the scale is connected and responding. */
  const int dataPin;                                /**< Data pin for HX711. */
  const int clockPin;                              /**< Clock pin for HX711. */

#ifndef UNIT_TEST
  Logger* logger = nullptr;                         /**< Logger for recording events. */
#endif

public:
#ifdef UNIT_TEST
  /**
   * Constructor for the Scale class in test environment.
   * @param dataPin The data pin for the HX711 module.
   * @param clockPin The clock pin for the HX711 module.
   */
  Scale(int dataPin, int clockPin) : dataPin(dataPin), clockPin(clockPin), connected(true) {
    for (float &reading : readings) {
      reading = 0.0F;
    }
  }
#else
  /**
   * Constructor for the Scale class.
   * @param dataPin The data pin for the HX711 module.
   * @param clockPin The clock pin for the HX711 module.
   * @param logger Pointer to the logger instance (optional).
   */
  Scale(int dataPin, int clockPin, Logger* logger = nullptr)
      : dataPin(dataPin), clockPin(clockPin), logger(logger) {

    if (logger) {
      logger->info("Initializing scale on pins %d, %d", dataPin, clockPin);
    }

    // Initialize reading array with zeros
    for (float &reading : readings) {
      reading = 0.0F;
    }

    // Try to connect with timeout
    unsigned long startTime = millis();
    scale.begin(dataPin, clockPin);

    // Check if scale responds within timeout
    connected = false;
    while (!scale.is_ready()) {
      if (millis() - startTime > SCALE_CONNECTION_TIMEOUT_MS) {
        if (logger) {
          logger->error("Scale connection timeout on pins %d, %d", dataPin, clockPin);
        }
        return;
      }
      delay(10);
    }

    // Scale is connected
    connected = true;
    if (logger) {
      logger->info("Scale connected successfully on pins %d, %d", dataPin, clockPin);
    }

    // Tare the scale
    scale.tare();
    if (logger) {
      logger->info("Scale tared on pins %d, %d", dataPin, clockPin);
    }
  }
#endif

  /**
   * Updates the weight reading.
   * @return True if weight was successfully updated, false otherwise.
   */
  bool updateWeight() {
#ifndef UNIT_TEST
    // Skip if not connected
    if (!connected) {
      if (logger) {
        logger->warning("Skipping update for disconnected scale on pins %d, %d", dataPin, clockPin);
      }
      return false;
    }

    // Wait for scale with timeout
    unsigned long startTime = millis();
    while (!scale.is_ready()) {
      if (millis() - startTime > SCALE_READ_TIMEOUT_MS) {
        if (logger) {
          logger->error("Timeout waiting for scale data on pins %d, %d", dataPin, clockPin);
        }
        connected = false;  // Mark as disconnected for future calls
        return false;
      }
      delay(10);
    }

    // Read weight
    float value = scale.get_units();
    readings[index] = value;
    index = (index + 1) % READINGS_ARRAY_SIZE;

    if (logger && logger->isLevelEnabled(Logger::DEBUG)) {
      logger->debug("Scale reading: %.2f on pins %d, %d", value, dataPin, clockPin);
    }
    return true;
#else
    // Test environment - just update
    readings[index] = scale.get_units();
    index = (index + 1) % READINGS_ARRAY_SIZE;
    return true;
#endif
  }

  /**
   * Returns the current weight.
   * @return The current weight in the unit defined by the HX711 library.
   */
  [[nodiscard]] float getWeight() const {
    std::array<float, READINGS_ARRAY_SIZE> sortedReadings{};
    std::copy(readings.begin(), readings.end(), sortedReadings.begin());
    std::sort(sortedReadings.begin(), sortedReadings.end());
    return sortedReadings[READINGS_ARRAY_MIDDLE_INDEX]; // return the median
  }

  /**
   * Returns the last weight reading.
   * @return The last weight reading in the unit defined by the HX711 library.
   */
  [[nodiscard]] float getLastWeight() const {
    int lastReadingIndex =
        (index - 1 + READINGS_ARRAY_SIZE) % READINGS_ARRAY_SIZE; // calculate the index of the last reading
    return readings[lastReadingIndex];
  }

  /**
   * Checks if the scale is connected and responding.
   * @return True if connected, false otherwise.
   */
  [[nodiscard]] bool isConnected() const {
    return connected;
  }

  /**
   * Attempts to reconnect to the scale.
   * @return True if successfully reconnected, false otherwise.
   */
  bool tryReconnect() {
#ifndef UNIT_TEST
    if (connected) return true; // Already connected

    if (logger) {
      logger->info("Attempting to reconnect scale on pins %d, %d", dataPin, clockPin);
    }

    // Try to connect with timeout
    unsigned long startTime = millis();
    scale.begin(dataPin, clockPin);

    // Check if scale responds within timeout
    while (!scale.is_ready()) {
      if (millis() - startTime > SCALE_CONNECTION_TIMEOUT_MS) {
        if (logger) {
          logger->error("Scale reconnection timeout on pins %d, %d", dataPin, clockPin);
        }
        return false;
      }
      delay(10);
    }

    // Scale is connected
    connected = true;
    scale.tare();

    if (logger) {
      logger->info("Scale reconnected successfully on pins %d, %d", dataPin, clockPin);
    }
    return true;
#else
    return true;
#endif
  }
};

#endif // SCALE_H
