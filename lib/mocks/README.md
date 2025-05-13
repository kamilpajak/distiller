# Hardware Mocks Library

This library provides mock implementations of Arduino hardware dependencies for use in:

1. Unit tests (`UNIT_TEST` defined)
2. Native builds (`NATIVE` defined)

## Purpose

The mocks allow code that depends on Arduino hardware to be compiled and tested on non-Arduino platforms. This enables:

- Unit testing with Google Test
- Native development and debugging on desktop platforms
- CI/CD pipelines without requiring physical hardware

## Key Components

- `MockArduino.h`: Provides mock implementations of Arduino core functionality
  - Declares basic Arduino constants (HIGH, LOW, etc.)
  - Implements basic Arduino functions (digitalWrite, pinMode, etc.)
  - Provides mock Stream and HardwareSerial classes
  - Includes a String class compatible with Arduino's

- Mock File System:
  - Mock `File` class with basic file operations
  - Mock `SD` class for SD card operations

## Usage

Include the mock headers in test or native code:

```cpp
#if defined(UNIT_TEST) || defined(NATIVE)
#include <MockArduino.h>
// Use mock implementations
#else
#include <Arduino.h>
// Use real hardware
#endif
```

## Notes

- The mocks are minimal implementations that allow code to compile and run
- They don't fully simulate hardware behavior
- Tests should use these mocks along with more specific test doubles for detailed testing