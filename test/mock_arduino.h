#ifndef MOCK_ARDUINO_H
#define MOCK_ARDUINO_H

#ifdef UNIT_TEST
#include <gmock/gmock.h>
#include <string.h>

// ArduinoMock namespace for time functions
namespace ArduinoMock {
// Mock millis() function
unsigned long millis();

// Function to set mock time (for test setup)
void setMillis(unsigned long newMillis);

// Function to advance mock time (for simulating time passing)
void advanceMillis(unsigned long millisToAdvance);
} // namespace ArduinoMock

// Global functions that delegate to ArduinoMock namespace
unsigned long millis();
void setMillis(unsigned long newMillis);
void advanceMillis(unsigned long millisToAdvance);

// Arduino constants
const int OUTPUT = 1;
const int HIGH = 1;
const int LOW = 0;
const int SS = 10;              // Default SPI SS pin
const int CHIP_SELECT_PIN = 10; // Default chip select pin for SD

#ifndef FILE_WRITE
#define FILE_WRITE "w" // File write mode
#endif

// Arduino delay() function
#ifndef DELAY_DEFINED
#define DELAY_DEFINED
inline void delay(unsigned long ms) {
  // No need to implement for tests
}
#endif

// Mock Serial class for unit tests
class SerialClass {
public:
  void begin(unsigned long baud) {}
  size_t print(const char *str) { return strlen(str); }
  size_t println(const char *str) { return strlen(str) + 1; }
  size_t print(float val, int format = 2) { return 1; }
  size_t println(float val, int format = 2) { return 2; }
  bool available() { return false; }
};

// Create a global Serial instance
static SerialClass Serial;

// ArduinoMockFixture class for pin operations
class ArduinoMockFixture {
public:
  MOCK_METHOD(void, Call, (int, int), (const));

  static ArduinoMockFixture &mockPinMode() {
    static ArduinoMockFixture instance;
    return instance;
  }

  static ArduinoMockFixture &mockDigitalWrite() {
    static ArduinoMockFixture instance;
    return instance;
  }

  static void reset() {
    testing::Mock::VerifyAndClearExpectations(&mockPinMode());
    testing::Mock::VerifyAndClearExpectations(&mockDigitalWrite());
  }
};

// Arduino function mocks
inline void pinMode(int pin, int mode) { ArduinoMockFixture::mockPinMode().Call(pin, mode); }

inline void digitalWrite(int pin, int value) { ArduinoMockFixture::mockDigitalWrite().Call(pin, value); }

#endif // UNIT_TEST
#endif // MOCK_ARDUINO_H
