#pragma once

#include <stdarg.h>
#include <stdint.h>

#ifndef UNIT_TEST
#include <Arduino.h>
#include <SD.h>
#include <HX711.h>
#endif

// Define some constants and functions for the test environment
#ifdef UNIT_TEST
// Arduino constants
#define SS 10
#define FILE_WRITE 0x02
#ifndef CHIP_SELECT_PIN
#define CHIP_SELECT_PIN 4
#endif

// Arduino functions - using inline to avoid multiple definition errors
inline unsigned long __attribute__((weak)) millis() { 
  static unsigned long fake_millis = 0;
  return fake_millis++; 
}
inline void __attribute__((weak)) delay(unsigned long) {}

// Forward declare File for test environment
class File {
public:
  bool operator!() const { return false; }
  operator bool() const { return true; }
  size_t println(const char*) { return 0; }
  void flush() {}
};
#endif

// Interface for Serial-like communication
class ISerialInterface {
public:
  virtual ~ISerialInterface() = default;
  virtual void begin(unsigned long baud) = 0;
  virtual size_t print(const char* str) = 0;
  virtual size_t println(const char* str) = 0;
  virtual size_t print(float val, int format = 2) = 0;
  virtual size_t println(float val, int format = 2) = 0;
  virtual bool available() = 0;
};

// Interface for SD card operations
class ISDInterface {
public:
  virtual ~ISDInterface() = default;
  virtual bool begin(uint8_t csPin = SS) = 0;
  virtual File open(const char* filename, uint8_t mode) = 0;
  virtual bool exists(const char* filename) = 0;
  virtual bool mkdir(const char* filename) = 0;
};

// Interface for Scale (HX711) operations
class IScaleInterface {
public:
  virtual ~IScaleInterface() = default;
  virtual void begin() = 0;
  virtual bool is_ready() = 0;
  virtual void set_scale(float scale) = 0;
  virtual void tare(uint8_t times = 10) = 0;
  virtual float get_units(uint8_t times = 10) = 0;
  virtual void power_down() = 0;
  virtual void power_up() = 0;
};

#ifndef UNIT_TEST
// Concrete implementation of SerialInterface that wraps the actual Serial object
class ArduinoSerialInterface : public ISerialInterface {
public:
  void begin(unsigned long baud) override {
    Serial.begin(baud);
  }
  
  size_t print(const char* str) override {
    return Serial.print(str);
  }
  
  size_t println(const char* str) override {
    return Serial.println(str);
  }
  
  size_t print(float val, int format = 2) override {
    return Serial.print(val, format);
  }
  
  size_t println(float val, int format = 2) override {
    return Serial.println(val, format);
  }
  
  bool available() override {
    return Serial.available() > 0;
  }
};

// Concrete implementation of SDInterface that wraps the actual SD library
class ArduinoSDInterface : public ISDInterface {
public:
  bool begin(uint8_t csPin = SS) override {
    return SD.begin(csPin);
  }
  
  File open(const char* filename, uint8_t mode) override {
    return SD.open(filename, mode);
  }
  
  bool exists(const char* filename) override {
    return SD.exists(filename);
  }
  
  bool mkdir(const char* filename) override {
    return SD.mkdir(filename);
  }
};

// Concrete implementation of ScaleInterface that wraps the HX711 library
class HX711ScaleInterface : public IScaleInterface {
private:
  HX711 scale;
  int dataPin;
  int clockPin;

public:
  HX711ScaleInterface(int dout, int sck) : dataPin(dout), clockPin(sck) {
    scale = HX711();
  }

  void begin() override {
    scale.begin(dataPin, clockPin);
  }
  
  bool is_ready() override {
    return scale.is_ready();
  }
  
  void set_scale(float scaleValue) override {
    scale.set_scale(scaleValue);
  }
  
  void tare(uint8_t times = 10) override {
    scale.tare(times);
  }
  
  float get_units(uint8_t times = 10) override {
    return scale.get_units(times);
  }
  
  void power_down() override {
    scale.power_down();
  }
  
  void power_up() override {
    scale.power_up();
  }
};
#endif // UNIT_TEST