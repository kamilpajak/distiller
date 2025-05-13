#ifndef SCALE_H
#define SCALE_H

#include "constants.h"
#include "hardware_interfaces.h"
#include "logger.h"

#include <algorithm>
#include <array>

/**
 * Class for managing a scale.
 */
class Scale {
private:
  IScaleInterface *scaleInterface;                   /**< Interface for HX711 operations. */
  std::array<float, READINGS_ARRAY_SIZE> readings{}; /**< Array to store weight readings. */
  int index{0};                                      /**< Index for the current reading. */
  bool connected{false};                             /**< Whether the scale is connected and responding. */
  const int dataPin;                                 /**< Data pin for HX711. */
  const int clockPin;                                /**< Clock pin for HX711. */
  Logger *logger = nullptr;                          /**< Logger for recording events. */

public:
  /**
   * Constructor for the Scale class.
   * @param scaleInterface Interface for HX711 operations.
   * @param dataPin The data pin for the HX711 module.
   * @param clockPin The clock pin for the HX711 module.
   * @param logger Pointer to the logger instance (optional).
   */
  Scale(IScaleInterface *scaleInterface, int dataPin, int clockPin, Logger *logger = nullptr);

  /**
   * Updates the weight reading.
   * @return True if weight was successfully updated, false otherwise.
   */
  bool updateWeight();

  /**
   * Returns the current weight.
   * @return The current weight in the unit defined by the HX711 library.
   */
  [[nodiscard]] float getWeight() const;

  /**
   * Returns the last weight reading.
   * @return The last weight reading in the unit defined by the HX711 library.
   */
  [[nodiscard]] float getLastWeight() const;

  /**
   * Checks if the scale is connected and responding.
   * @return True if connected, false otherwise.
   */
  [[nodiscard]] bool isConnected() const;

  /**
   * Attempts to reconnect to the scale.
   * @return True if successfully reconnected, false otherwise.
   */
  bool tryReconnect();
};

#endif // SCALE_H