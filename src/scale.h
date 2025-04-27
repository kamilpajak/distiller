#ifndef SCALE_H
#define SCALE_H

#ifndef UNIT_TEST
#include "../include/HX711.h"
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

public:
  /**
   * Constructor for the Scale class.
   * @param dataPin The data pin for the HX711 module.
   * @param clockPin The clock pin for the HX711 module.
   */
  Scale(int dataPin, int clockPin) {
    scale.begin(dataPin, clockPin);
    for (float &reading : readings) {
      reading = 0.0F;
    }
    scale.tare();
  }

  /**
   * Updates the weight reading.
   */
  void updateWeight() {
    readings[index] = scale.get_units();
    index = (index + 1) % READINGS_ARRAY_SIZE;
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
};

#endif // SCALE_H
