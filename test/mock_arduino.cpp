#ifdef UNIT_TEST
#include "mock_arduino.h"

// Global variable to control mock time
unsigned long mock_millis_time = 0;

// Mock millis() function
unsigned long millis() { return mock_millis_time; }

// Function to set mock time (for test setup)
void setMillis(unsigned long newMillis) { mock_millis_time = newMillis; }

// Function to advance mock time (for simulating time passing)
void advanceMillis(unsigned long millisToAdvance) { mock_millis_time += millisToAdvance; }

#endif // UNIT_TEST
