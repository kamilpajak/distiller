#ifndef MOCK_ARDUINO_H
#define MOCK_ARDUINO_H

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>

// Define NATIVE for native builds if not already defined in platformio.ini
#if defined(PLATFORMIO) && !defined(ARDUINO) && !defined(UNIT_TEST)
#define NATIVE
#endif

// Define File class and SD functionality for non-production builds
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

#ifndef SDClass_defined
#define SDClass_defined
class SDClass {
public:
  bool begin(uint8_t csPin = 10) { return true; }
  File open(const char *filename, const char *mode) { return File(); }
  bool exists(const char *filename) { return true; }
  bool mkdir(const char *filename) { return true; }
};
extern SDClass SD;
#endif

#if defined(UNIT_TEST)
// Use existing mock_arduino.h for unit tests
#include "mock_arduino.h"
#elif defined(NATIVE)
// Comprehensive Arduino mock for native builds

// Basic Arduino constants
#define HIGH 0x1
#define LOW 0x0

#define INPUT 0x0
#define OUTPUT 0x1
#define INPUT_PULLUP 0x2
#define SS 10
#define CHIP_SELECT_PIN 10
#define FILE_WRITE "w"
#define FILE_READ "r"
#define FILE_APPEND "a"

// Serial print formatting
#define DEC 10
#define HEX 16
#define OCT 8
#define BIN 2

// Forward declarations
class HardwareSerial;
extern HardwareSerial Serial;

// Basic Arduino functions
extern "C" {
// Time functions
static unsigned long _mock_millis = 0;
unsigned long millis() { return _mock_millis; }
void delay(unsigned long ms) { _mock_millis += ms; }

// Pin I/O functions
void pinMode(uint8_t pin, uint8_t mode) {}
void digitalWrite(uint8_t pin, uint8_t val) {}
int digitalRead(uint8_t pin) { return LOW; }
int analogRead(uint8_t pin) { return 0; }
void analogWrite(uint8_t pin, int val) {}

// Advanced time functions
unsigned long micros() { return _mock_millis * 1000UL; }
void delayMicroseconds(unsigned int us) { _mock_millis += us / 1000UL; }
}

// Serial communication
class Stream {
public:
  virtual size_t write(uint8_t) = 0;
  virtual int available() = 0;
  virtual int read() = 0;
  virtual int peek() = 0;
  virtual void flush() = 0;

  size_t print(const char *str) { return strlen(str); }
  size_t print(int num, int base = DEC) { return num == 0 ? 1 : (int)log10(abs(num)) + 1 + (num < 0 ? 1 : 0); }
  size_t print(unsigned int num, int base = DEC) { return num == 0 ? 1 : (int)log10(num) + 1; }
  size_t print(long num, int base = DEC) { return num == 0 ? 1 : (int)log10(abs(num)) + 1 + (num < 0 ? 1 : 0); }
  size_t print(unsigned long num, int base = DEC) { return num == 0 ? 1 : (int)log10(num) + 1; }
  size_t print(double num, int digits = 2) { return 10; }

  size_t println(const char *str) { return print(str) + 2; }
  size_t println(int num, int base = DEC) { return print(num, base) + 2; }
  size_t println(unsigned int num, int base = DEC) { return print(num, base) + 2; }
  size_t println(long num, int base = DEC) { return print(num, base) + 2; }
  size_t println(unsigned long num, int base = DEC) { return print(num, base) + 2; }
  size_t println(double num, int digits = 2) { return print(num, digits) + 2; }
  size_t println(void) { return 2; }
};

class HardwareSerial : public Stream {
public:
  void begin(unsigned long baud) {}
  void end() {}
  virtual size_t write(uint8_t byte) override { return 1; }
  virtual int available() override { return 0; }
  virtual int read() override { return -1; }
  virtual int peek() override { return -1; }
  virtual void flush() override {}
};

// String class for Arduino compatibility
class String {
private:
  std::string str;

public:
  String() : str("") {}
  String(const char *s) : str(s) {}
  String(const std::string &s) : str(s) {}

  // Constructors for numeric types
  String(int value, int base = 10) : str(std::to_string(value)) {}
  String(unsigned int value, int base = 10) : str(std::to_string(value)) {}
  String(long value, int base = 10) : str(std::to_string(value)) {}
  String(unsigned long value, int base = 10) : str(std::to_string(value)) {}
  String(float value, int decimalPlaces = 2) : str(std::to_string(value)) {}
  String(double value, int decimalPlaces = 2) : str(std::to_string(value)) {}

  // String concatenation
  String operator+(const String &rhs) const { return String((str + rhs.str).c_str()); }
  String operator+(const char *rhs) const { return String((str + rhs).c_str()); }
  friend String operator+(const char *lhs, const String &rhs) { return String((std::string(lhs) + rhs.str).c_str()); }

  const char *c_str() const { return str.c_str(); }
  operator const char *() const { return str.c_str(); }

  // Length methods
  unsigned int length() const { return str.length(); }

  // Comparison
  bool equals(const String &s) const { return str == s.str; }
  bool equals(const char *s) const { return str == s; }
  bool operator==(const String &rhs) const { return equals(rhs); }
  bool operator!=(const String &rhs) const { return !equals(rhs); }
};

// Global serial instance
static HardwareSerial SerialInstance;
HardwareSerial Serial = SerialInstance;

// Global SD instance
#ifndef SDClass_defined
#define SDClass_defined
class SDClass {
public:
  bool begin(uint8_t csPin = 10) { return true; }
  File open(const char *filename, const char *mode) { return File(); }
  bool exists(const char *filename) { return true; }
  bool mkdir(const char *filename) { return true; }
};
#endif
static SDClass SDInstance;
SDClass SD = SDInstance;

#else
// Minimal Arduino.h for non-test, non-native builds
#include <cstdint>
#include <string>

// Function declarations
unsigned long millis();

// Constants
const int OUTPUT = 1;
const int HIGH = 1;
const int LOW = 0;

// Function declarations
void pinMode(int pin, int mode);
void digitalWrite(int pin, int value);
void delay(unsigned long ms);

// String class for Arduino compatibility
class String {
private:
  std::string str;

public:
  String() : str("") {}
  String(const char *s) : str(s) {}
  String(const std::string &s) : str(s) {}

  // Constructors for numeric types
  String(int value, int base = 10) : str(std::to_string(value)) {}
  String(unsigned int value, int base = 10) : str(std::to_string(value)) {}
  String(long value, int base = 10) : str(std::to_string(value)) {}
  String(unsigned long value, int base = 10) : str(std::to_string(value)) {}
  String(float value, int decimalPlaces = 2) : str(std::to_string(value)) {}
  String(double value, int decimalPlaces = 2) : str(std::to_string(value)) {}

  // Constructor for enum types
  template <typename T>
  String(T value, typename std::enable_if<std::is_enum<T>::value>::type * = nullptr)
    : str(std::to_string(static_cast<int>(value))) {}

  // String concatenation
  String operator+(const String &rhs) const { return String((str + rhs.str).c_str()); }
  String operator+(const char *rhs) const { return String((str + rhs).c_str()); }
  friend String operator+(const char *lhs, const String &rhs) { return String((std::string(lhs) + rhs.str).c_str()); }

  const char *c_str() const { return str.c_str(); }
  operator const char *() const { return str.c_str(); }
};
#endif

#endif // MOCK_ARDUINO_H