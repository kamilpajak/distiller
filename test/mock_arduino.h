#ifdef UNIT_TEST
// Mock Arduino functions for testing in a native environment

// Global variable to control mock time
extern unsigned long mock_millis_time;

// Mock millis() function
unsigned long millis();

// Function to set mock time (for test setup)
void setMillis(unsigned long newMillis);

// Function to advance mock time (for simulating time passing)
void advanceMillis(unsigned long millisToAdvance);

#endif // UNIT_TEST
