#ifndef SCALE_H
#define SCALE_H

#ifndef UNIT_TEST
#include <HX711.h>
#endif
#include <algorithm>

/**
 * Class for managing a scale.
 */
class Scale {
private:
  HX711 scale;       /**< HX711 object for weight measurement. */
  float readings[5]; /**< Array to store weight readings. */
  int index;         /**< Index for the current reading. */

public:
  /**
   * Constructor for the Scale class.
   * @param dataPin The data pin for the HX711 module.
   * @param clockPin The clock pin for the HX711 module.
   */
  Scale(int dataPin, int clockPin) : index(0) {
    scale.begin(dataPin, clockPin);
    for (int i = 0; i < 5; i++) {
      readings[i] = 0.0;
    }
    scale.tare();
  }

  /**
   * Updates the weight reading.
   */
  void updateWeight() {
    readings[index] = scale.get_units();
    index = (index + 1) % 5;
  }

  /**
   * Returns the current weight.
   * @return The current weight in the unit defined by the HX711 library.
   */
  float getWeight() const {
    float sortedReadings[5];
    std::copy(readings, readings + 5, sortedReadings);
    std::sort(sortedReadings, sortedReadings + 5);
    return sortedReadings[2]; // return the median
  }

  /**
   * Returns the last weight reading.
   * @return The last weight reading in the unit defined by the HX711 library.
   */
  float getLastWeight() {
    int lastReadingIndex = (index - 1 + 5) % 5; // calculate the index of the last reading
    return readings[lastReadingIndex];
  }
};

#endif // SCALE_H
