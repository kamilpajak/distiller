#pragma once

#include <stdarg.h>
#include <stdint.h>
#include <string.h>

// Define NATIVE for native builds if not already defined
#if defined(PLATFORMIO) && !defined(ARDUINO) && !defined(UNIT_TEST)
#define NATIVE
#endif

// Forward declarations of Arduino types for production builds
// These symbols are defined in the real Arduino libraries
#if !defined(UNIT_TEST) && !defined(NATIVE)
    // These are already defined in the real Arduino.h
    class HardwareSerial;
    extern HardwareSerial Serial;

    // File is defined in SD.h
    class File;

    // SDClass is defined in SD.h
    class SDClass;
    extern SDClass SD;

    // HX711 is defined in HX711.h
    class HX711;

    // Define SS if not already defined
    #ifndef SS
    #define SS 10
    #endif

    // FILE_WRITE for SD card operations
    #ifndef FILE_WRITE
    #define FILE_WRITE "w"
    #endif
#else
    // For test/native builds, we use our mock implementations
    #include <Arduino.h>

    // Define File class for test/native builds
    #ifndef File_defined
    #define File_defined
    class File {
    public:
      File() {}
      operator bool() const { return true; }
      bool println(const char *message) { return true; }
      bool print(const char *message) { return true; }
      bool flush() { return true; }
    };
    #endif

    // Define SDClass for test/native builds
    #ifndef SDClass_defined
    #define SDClass_defined
    class SDClass {
    public:
      bool begin(uint8_t csPin = SS) { return true; }
      File open(const char *filename, const char *mode) { return File(); }
      bool exists(const char *filename) { return true; }
      bool mkdir(const char *filename) { return true; }
    };
    extern SDClass SD;
    #endif

    // Define SS if not already defined
    #ifndef SS
    #define SS 10
    #endif

    // FILE_WRITE for SD card operations
    #ifndef FILE_WRITE
    #define FILE_WRITE "w"
    #endif
#endif

/**
 * @file hardware_interfaces.h
 * @brief Hardware abstraction interfaces for the distillation control system.
 *
 * This file defines a set of interfaces to abstract hardware-specific
 * functionality, which enables better testability and maintainability.
 * The abstractions allow the business logic to be decoupled from the
 * specific hardware implementations.
 */

// Mock HX711 class when building for test or native environments
#if defined(UNIT_TEST) || defined(NATIVE)
/**
 * @brief Mock implementation of HX711 library for test and native builds.
 *
 * Provides a simulation of load cell amplifier functionality without
 * requiring actual hardware connections.
 */
class HX711 {
private:
  float mockWeight = 0.0f;
  float scaleCalibration = 1.0f;

public:
  HX711() {}

  void begin(int dataPin, int clockPin) {}

  // Naming consistency fix: the actual implementation has undercores in method names
  bool is_ready() { return true; }
  bool isReady() { return true; } // Alternative name for consistency

  void set_scale(float scale) { scaleCalibration = scale; }

  void tare(uint8_t times = 10) { mockWeight = 0.0f; }

  void power_down() {}
  void powerDown() {} // Alternative name for consistency

  void power_up() {}
  void powerUp() {} // Alternative name for consistency

  float get_units(uint8_t times = 10) {
    // Simulate some variation in readings
    static int counter = 0;
    counter++;
    return (mockWeight + (counter % 3) * 0.01) / scaleCalibration;
  }

  // Method to set mock weight for testing
  void set_mock_weight(float weight) { mockWeight = weight; }
};
#endif

/**
 * @brief Interface for Serial communication.
 *
 * This interface abstracts serial communication operations, allowing
 * the system to work with different serial implementations or mocks
 * during testing.
 */
class ISerialInterface {
public:
  /** Virtual destructor for proper cleanup */
  virtual ~ISerialInterface() = default;

  /**
   * @brief Initialize serial communication.
   * @param baud - Baud rate for serial communication
   */
  virtual void begin(unsigned long baud) = 0;

  /**
   * @brief Print string to serial output.
   * @param str - String to print
   * @return Number of bytes written
   */
  virtual size_t print(const char *str) = 0;

  /**
   * @brief Print string with newline to serial output.
   * @param str - String to print
   * @return Number of bytes written
   */
  virtual size_t println(const char *str) = 0;

  /**
   * @brief Print floating-point value to serial output.
   * @param val - Value to print
   * @param format - Decimal precision
   * @return Number of bytes written
   */
  virtual size_t print(float val, int format = 2) = 0;

  /**
   * @brief Print floating-point value with newline to serial output.
   * @param val - Value to print
   * @param format - Decimal precision
   * @return Number of bytes written
   */
  virtual size_t println(float val, int format = 2) = 0;

  /**
   * @brief Check if serial data is available to read.
   * @return true if data available, false otherwise
   */
  virtual bool available() = 0;
};

/**
 * @brief Interface for SD card operations.
 *
 * This interface abstracts SD card operations, allowing the system
 * to work with different SD implementations or mocks during testing.
 */
class ISDInterface {
public:
  /** Virtual destructor for proper cleanup */
  virtual ~ISDInterface() = default;

  /**
   * @brief Initialize SD card.
   * @param csPin - Chip select pin for SPI communication
   * @return true if initialization successful, false otherwise
   */
  virtual bool begin(uint8_t csPin = 10) = 0; // Use 10 as default SS

/**
 * @brief Open a file on the SD card.
 * @param filename - Name of file to open
 * @param mode - File mode (read/write) - can be a string (like FILE_WRITE) or uint8_t constant
 * @return File object
 */
  virtual File open(const char *filename, const char *mode) = 0;

  /**
   * @brief Check if a file exists on the SD card.
   * @param filename - Name of file to check
   * @return true if file exists, false otherwise
   */
  virtual bool exists(const char *filename) = 0;

  /**
   * @brief Create a directory on the SD card.
   * @param filename - Path of directory to create
   * @return true if directory created, false otherwise
   */
  virtual bool mkdir(const char *filename) = 0;
};

/**
 * @brief Interface for scale (load cell) operations.
 *
 * This interface abstracts scale operations using HX711 load cell amplifiers,
 * allowing the system to work with different implementations or mocks during testing.
 */
class IScaleInterface {
public:
  /** Virtual destructor for proper cleanup */
  virtual ~IScaleInterface() = default;

  /**
   * @brief Initialize the scale.
   */
  virtual void begin() = 0;

  /**
   * @brief Check if the scale is ready to provide readings.
   * @return true if scale is ready, false otherwise
   */
  virtual bool is_ready() = 0;

  /**
   * @brief Set the scale calibration factor.
   * @param scale - Calibration factor
   */
  virtual void set_scale(float scale) = 0;

  /**
   * @brief Tare the scale (set current reading as zero).
   * @param times - Number of readings to average for tare
   */
  virtual void tare(uint8_t times = 10) = 0;

  /**
   * @brief Get weight reading in calibrated units.
   * @param times - Number of readings to average
   * @return Weight reading
   */
  virtual float get_units(uint8_t times = 10) = 0;

  /**
   * @brief Power down the scale to save power.
   */
  virtual void power_down() = 0;

  /**
   * @brief Power up the scale.
   */
  virtual void power_up() = 0;
};

/**
 * @brief Arduino implementation of the Serial interface.
 *
 * This class provides an implementation of ISerialInterface that
 * wraps Arduino's built-in Serial object.
 */
class ArduinoSerialInterface : public ISerialInterface {
public:
  // These implementations are defined in the .cpp file to avoid direct use of Arduino.h here
  void begin(unsigned long baud) override;
  size_t print(const char *str) override;
  size_t println(const char *str) override;
  size_t print(float val, int format = 2) override;
  size_t println(float val, int format = 2) override;
  bool available() override;
};

/**
 * @brief Arduino implementation of the SD card interface.
 *
 * This class provides an implementation of ISDInterface that
 * wraps Arduino's SD library.
 */
class ArduinoSDInterface : public ISDInterface {
public:
  // These implementations are defined in the .cpp file to avoid direct use of SD.h here
  bool begin(uint8_t csPin = 10) override;
  File open(const char *filename, const char *mode) override;
  bool exists(const char *filename) override;
  bool mkdir(const char *filename) override;
};

/**
 * @brief HX711 implementation of the scale interface.
 *
 * This class provides an implementation of IScaleInterface that
 * wraps the HX711 library for weight measurement.
 */
class HX711ScaleInterface : public IScaleInterface {
private:
#if defined(UNIT_TEST) || defined(NATIVE)
  // For test/native builds, use the mock implementation
  class HX711 {
  private:
    float mockWeight = 0.0f;
    float scaleCalibration = 1.0f;

  public:
    HX711() {}
    void begin(int dataPin, int clockPin) {}
    bool isReady() { return true; }
    void set_scale(float scale) { scaleCalibration = scale; }
    void tare(uint8_t times = 10) { mockWeight = 0.0f; }
    void powerDown() {}
    void powerUp() {}
    float get_units(uint8_t times = 10) {
      // Simulate some variation in readings
      static int counter = 0;
      counter++;
      return (mockWeight + (counter % 3) * 0.01) / scaleCalibration;
    }
    void set_mock_weight(float weight) { mockWeight = weight; }
  };
  HX711 scale;  /**< Underlying mock HX711 object */
#else 
  // For production builds, HX711 is defined in HX711.h included in main.cpp
  // The implementation is in the .cpp file
  void* scalePtr; // We'll cast this to HX711* in the implementation
#endif
  int dataPin;  /**< Data pin for HX711 module */
  int clockPin; /**< Clock pin for HX711 module */

public:
  /**
   * @brief Construct a new HX711 Scale Interface object.
   *
   * @param dout - Data output pin for HX711
   * @param sck - Serial clock pin for HX711
   */
  HX711ScaleInterface(int dout, int sck);
  ~HX711ScaleInterface();

  void begin() override;
  bool is_ready() override;
  void set_scale(float scaleValue) override;
  void tare(uint8_t times = 10) override;
  float get_units(uint8_t times = 10) override;
  void power_down() override;
  void power_up() override;

#if defined(UNIT_TEST) || defined(NATIVE)
  /**
   * @brief Set mock weight for testing (only available in test/native builds).
   *
   * This method allows test code to set a specific weight value to be
   * returned by the scale when get_units() is called.
   *
   * @param weight - The weight value to simulate
   */
  void set_mock_weight(float weight) { scale.set_mock_weight(weight); }
#endif
};