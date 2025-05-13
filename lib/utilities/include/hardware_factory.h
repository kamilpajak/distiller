#pragma once

#include <hardware_interfaces.h>

#if !defined(UNIT_TEST) && !defined(NATIVE)
/**
 * Factory class for creating hardware interface implementations.
 * This helps with dependency injection in production code while
 * allowing mock implementations for testing.
 */
class HardwareFactory {
public:
  /**
   * Get a Serial interface implementation.
   * @return Pointer to a SerialInterface implementation.
   */
  static ISerialInterface *getSerialInterface() {
    static ArduinoSerialInterface serialInterface;
    return &serialInterface;
  }

  /**
   * Get an SD card interface implementation.
   * @return Pointer to an SDInterface implementation.
   */
  static ISDInterface *getSDInterface() {
    static ArduinoSDInterface sdInterface;
    return &sdInterface;
  }

  /**
   * Create a new Scale interface implementation.
   * @param dataPin The data pin for the HX711 module.
   * @param clockPin The clock pin for the HX711 module.
   * @return Pointer to a newly created ScaleInterface implementation.
   * @note The caller is responsible for deleting the returned pointer.
   */
  static IScaleInterface *createScaleInterface(int dataPin, int clockPin) {
    return new HX711ScaleInterface(dataPin, clockPin);
  }
};
#else
/**
 * Mock factory class for creating hardware interface implementations in testing/native environments.
 */
class HardwareFactory {
public:
  /**
   * Get a Serial interface implementation.
   * @return Pointer to a SerialInterface implementation.
   */
  static ISerialInterface *getSerialInterface() {
    static ArduinoSerialInterface serialInterface;
    return &serialInterface;
  }

  /**
   * Get an SD card interface implementation.
   * @return Pointer to an SDInterface implementation.
   */
  static ISDInterface *getSDInterface() {
    static ArduinoSDInterface sdInterface;
    return &sdInterface;
  }

  /**
   * Create a new Scale interface implementation.
   * @param dataPin The data pin for the HX711 module.
   * @param clockPin The clock pin for the HX711 module.
   * @return Pointer to a newly created ScaleInterface implementation.
   * @note The caller is responsible for deleting the returned pointer.
   */
  static IScaleInterface *createScaleInterface(int dataPin, int clockPin) {
    return new HX711ScaleInterface(dataPin, clockPin);
  }
};
#endif // UNIT_TEST and NATIVE