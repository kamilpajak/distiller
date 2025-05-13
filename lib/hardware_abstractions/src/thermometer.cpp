#include "../include/thermometer.h"

#ifdef UNIT_TEST
// Constructor for the test environment
Thermometer::Thermometer(std::shared_ptr<MockOneWire> ow, std::shared_ptr<MockDallasTemperature> ds)
  : oneWire(std::move(ow)), sensors(std::move(ds)) {
  for (float &reading : readings) {
    reading = 0.0F;
  }
}

// Method to set lastMedian for testing purposes
void Thermometer::setLastMedian(float median) { lastMedian = median; }
#else
// Constructor for the Thermometer class
Thermometer::Thermometer(int pin) : oneWire(pin), sensors(&oneWire) {
  sensors.begin();
  for (float &reading : readings) {
    reading = 0.0F;
  }
}
#endif

// Updates the temperature reading
void Thermometer::updateTemperature() {
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

// Checks if there is a sudden temperature increase beyond a given threshold
bool Thermometer::isSuddenTemperatureIncrease(float threshold) const {
  if (readingsCount < READINGS_ARRAY_SIZE) {
    return false; // Not enough readings to calculate the median
  }
  float currentMedian = getTemperature();
  float difference = currentMedian - lastMedian;
  return difference > threshold + TEMPERATURE_COMPARISON_TOLERANCE;
}

// Returns the current temperature
float Thermometer::getTemperature() const {
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

// Returns the last temperature reading
float Thermometer::getLastTemperature() const {
  int lastIndex = (index - 1 + READINGS_ARRAY_SIZE) % READINGS_ARRAY_SIZE; // calculate the index of the last reading
  return readings[lastIndex]; // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
}