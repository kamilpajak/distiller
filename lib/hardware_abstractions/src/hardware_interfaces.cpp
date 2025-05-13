#include <hardware_interfaces.h>

// Implementations for the hardware interfaces
// These implementations avoid directly using Arduino symbols in header files

// For production environment, include the real libraries
#if !defined(NATIVE) && !defined(UNIT_TEST)
// Explicitly include the real Arduino headers here in the .cpp file, not in the .h file
#include <Arduino.h> // Real Arduino.h from framework
#include <HX711.h>   // Real HX711.h from library
#include <SD.h>      // Real SD.h from framework
#include <SPI.h>     // Real SPI.h from framework

// ArduinoSerialInterface implementations for production
void ArduinoSerialInterface::begin(unsigned long baud) { Serial.begin(baud); }

size_t ArduinoSerialInterface::print(const char *str) { return Serial.print(str); }

size_t ArduinoSerialInterface::println(const char *str) { return Serial.println(str); }

size_t ArduinoSerialInterface::print(float val, int format) { return Serial.print(val, format); }

size_t ArduinoSerialInterface::println(float val, int format) { return Serial.println(val, format); }

bool ArduinoSerialInterface::available() { return Serial.available() > 0; }

// ArduinoSDInterface implementations for production
bool ArduinoSDInterface::begin(uint8_t csPin) { return SD.begin(csPin); }

File ArduinoSDInterface::open(const char *filename, const char *mode) { return SD.open(filename, mode); }

bool ArduinoSDInterface::exists(const char *filename) { return SD.exists(filename); }

bool ArduinoSDInterface::mkdir(const char *filename) { return SD.mkdir(filename); }

// HX711ScaleInterface implementations for production
HX711ScaleInterface::HX711ScaleInterface(int dout, int sck) : dataPin(dout), clockPin(sck) {
  // Create a new HX711 object
  scalePtr = new HX711();
}

HX711ScaleInterface::~HX711ScaleInterface() {
  if (scalePtr) {
    delete static_cast<HX711 *>(scalePtr);
    scalePtr = nullptr;
  }
}

void HX711ScaleInterface::begin() { static_cast<HX711 *>(scalePtr)->begin(dataPin, clockPin); }

bool HX711ScaleInterface::is_ready() { return static_cast<HX711 *>(scalePtr)->is_ready(); }

void HX711ScaleInterface::set_scale(float scaleValue) { static_cast<HX711 *>(scalePtr)->set_scale(scaleValue); }

void HX711ScaleInterface::tare(uint8_t times) { static_cast<HX711 *>(scalePtr)->tare(times); }

float HX711ScaleInterface::get_units(uint8_t times) { return static_cast<HX711 *>(scalePtr)->get_units(times); }

void HX711ScaleInterface::power_down() { static_cast<HX711 *>(scalePtr)->power_down(); }

void HX711ScaleInterface::power_up() { static_cast<HX711 *>(scalePtr)->power_up(); }

#else
// For test/native environments, we use mock implementations
// We include the Arduino mock header for test/native builds
#include <Arduino.h> // This will be the mock Arduino.h

// ArduinoSerialInterface implementations for test/native
void ArduinoSerialInterface::begin(unsigned long baud) {
  // In the mock environment, we don't need to do anything real
  // Serial is provided by MockArduino.h
}

size_t ArduinoSerialInterface::print(const char *str) {
  // For mocks, just return the length that would have been printed
  return strlen(str);
}

size_t ArduinoSerialInterface::println(const char *str) {
  // For mocks, return length + 2 for CR+LF
  return strlen(str) + 2;
}

size_t ArduinoSerialInterface::print(float val, int format) {
  // For mocks, return a reasonable size
  return 10;
}

size_t ArduinoSerialInterface::println(float val, int format) {
  // For mocks, return a reasonable size + 2 for CR+LF
  return 12;
}

bool ArduinoSerialInterface::available() {
  // For mocks, no data is available
  return false;
}

// ArduinoSDInterface mock implementations for test/native
bool ArduinoSDInterface::begin(uint8_t csPin) {
  // Mock always succeeds
  return true;
}

File ArduinoSDInterface::open(const char *filename, const char *mode) {
  // Return a mock File object
  return File();
}

bool ArduinoSDInterface::exists(const char *filename) {
  // Mock always returns true
  return true;
}

bool ArduinoSDInterface::mkdir(const char *filename) {
  // Mock always succeeds
  return true;
}

// HX711ScaleInterface implementations for test/native
HX711ScaleInterface::HX711ScaleInterface(int dout, int sck) : dataPin(dout), clockPin(sck) {
  // In test/native builds, scale is already defined in the class as a mock HX711
}

HX711ScaleInterface::~HX711ScaleInterface() {
  // Nothing to do in test/native builds
}

void HX711ScaleInterface::begin() {
  // Use the mock scale implementation
  scale.begin(dataPin, clockPin);
}

bool HX711ScaleInterface::is_ready() {
  // Use the mock isReady method
  return scale.isReady();
}

void HX711ScaleInterface::set_scale(float scaleValue) {
  // Use the mock set_scale method
  scale.set_scale(scaleValue);
}

void HX711ScaleInterface::tare(uint8_t times) {
  // Use the mock tare method
  scale.tare(times);
}

float HX711ScaleInterface::get_units(uint8_t times) {
  // Use the mock get_units method
  return scale.get_units(times);
}

void HX711ScaleInterface::power_down() {
  // Use the mock powerDown method
  scale.powerDown();
}

void HX711ScaleInterface::power_up() {
  // Use the mock powerUp method
  scale.powerUp();
}

// Define the global SD instance for test/native builds
SDClass SD;
#endif