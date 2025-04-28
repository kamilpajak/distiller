#ifndef MOCK_ARDUINO_H
#define MOCK_ARDUINO_H

#ifdef UNIT_TEST
#include <gmock/gmock.h>

// Global variable to control mock time
extern unsigned long mock_millis_time; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

// Mock millis() function
unsigned long millis();

// Function to set mock time (for test setup)
void setMillis(unsigned long newMillis);

// Function to advance mock time (for simulating time passing)
void advanceMillis(unsigned long millisToAdvance);

// Arduino constants
const int OUTPUT = 1;
const int HIGH = 1;
const int LOW = 0;

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
