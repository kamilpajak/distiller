#ifdef UNIT_TEST
#include "mock_arduino.h"

// Global variable to control mock time
namespace ArduinoMock {
static unsigned long currentMillis = 0; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

unsigned long millis() { return currentMillis; }

void setMillis(unsigned long newMillis) { currentMillis = newMillis; }

void advanceMillis(unsigned long millisToAdvance) { currentMillis += millisToAdvance; }
} // namespace ArduinoMock

// Mock millis() function
unsigned long millis() { return ArduinoMock::millis(); }

// Function to set mock time (for test setup)
void setMillis(unsigned long newMillis) { ArduinoMock::setMillis(newMillis); }

// Function to advance mock time (for simulating time passing)
void advanceMillis(unsigned long millisToAdvance) { ArduinoMock::advanceMillis(millisToAdvance); }

// Define the SD instance
#ifndef SDClass_defined
// Note: Since SDClass is already defined in hardware_interfaces.h, we don't define it here
#endif

// Define the SD instance
#ifdef SDClass_defined
SDClass SD;
#endif

#endif // UNIT_TEST
