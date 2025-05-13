#include "../include/scale.h"

#include "../../utilities/include/logger.h"

Scale::Scale(IScaleInterface *scaleInterface, int dataPin, int clockPin, Logger *logger)
  : scaleInterface(scaleInterface), dataPin(dataPin), clockPin(clockPin), logger(logger) {

  if (logger) {
    logger->info("Initializing scale on pins %d, %d", dataPin, clockPin);
  }

  // Initialize reading array with zeros
  for (float &reading : readings) {
    reading = 0.0F;
  }

  // Try to connect with timeout
  unsigned long startTime = millis();
  scaleInterface->begin();

  // Check if scale responds within timeout
  connected = false;
  while (!scaleInterface->is_ready()) {
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
  scaleInterface->tare();
  if (logger) {
    logger->info("Scale tared on pins %d, %d", dataPin, clockPin);
  }
}

bool Scale::updateWeight() {
  // Skip if not connected
  if (!connected) {
    if (logger) {
      logger->warning("Skipping update for disconnected scale on pins %d, %d", dataPin, clockPin);
    }
    return false;
  }

  // Wait for scale with timeout
  unsigned long startTime = millis();
  while (!scaleInterface->is_ready()) {
    if (millis() - startTime > SCALE_READ_TIMEOUT_MS) {
      if (logger) {
        logger->error("Timeout waiting for scale data on pins %d, %d", dataPin, clockPin);
      }
      connected = false; // Mark as disconnected for future calls
      return false;
    }
    delay(10);
  }

  // Read weight
  float value = scaleInterface->get_units();
  readings[index] = value;
  index = (index + 1) % READINGS_ARRAY_SIZE;

  if (logger && logger->isLevelEnabled(Logger::DEBUG_LEVEL)) {
    logger->debug("Scale reading: %.2f on pins %d, %d", value, dataPin, clockPin);
  }
  return true;
}

float Scale::getWeight() const {
  std::array<float, READINGS_ARRAY_SIZE> sortedReadings{};
  std::copy(readings.begin(), readings.end(), sortedReadings.begin());
  std::sort(sortedReadings.begin(), sortedReadings.end());
  return sortedReadings[READINGS_ARRAY_MIDDLE_INDEX]; // return the median
}

float Scale::getLastWeight() const {
  int lastReadingIndex =
      (index - 1 + READINGS_ARRAY_SIZE) % READINGS_ARRAY_SIZE; // calculate the index of the last reading
  return readings[lastReadingIndex];
}

bool Scale::isConnected() const { return connected; }

bool Scale::tryReconnect() {
  if (connected)
    return true; // Already connected

  if (logger) {
    logger->info("Attempting to reconnect scale on pins %d, %d", dataPin, clockPin);
  }

  // Try to connect with timeout
  unsigned long startTime = millis();
  scaleInterface->begin();

  // Check if scale responds within timeout
  while (!scaleInterface->is_ready()) {
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
  scaleInterface->tare();

  if (logger) {
    logger->info("Scale reconnected successfully on pins %d, %d", dataPin, clockPin);
  }
  return true;
}