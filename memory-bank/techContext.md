# Technical Context: Distiller

## Technologies Used

### Hardware Platform
- **Microcontroller**: Arduino MKR WiFi 1010 (ATSAMD21 + u-blox NINA-W10 WiFi module)
- **Temperature Sensors**: DS18B20 digital temperature sensors (4 units)
- **Weight Sensors**: HX711 load cell amplifiers with load cells (6 units)
- **Relays**: Digital relay modules for controlling heaters and valves (11 units)
- **Display**: I2C LCD display (20x4 characters)
- **I2C Multiplexer**: TCA9548A I2C multiplexer for managing multiple I2C devices

### Software Framework
- **Development Platform**: PlatformIO
- **Framework**: Arduino
- **Target Platform**: Atmel SAM (atmelsam)

### Libraries
- **HX711**: For interfacing with load cell amplifiers (version 0.7.5)
- **DallasTemperature**: For interfacing with DS18B20 temperature sensors (version 3.11.0)
- **PID**: For implementing PID control (version 1.2.1)
- **hd44780**: For controlling the LCD display (version 1.3.2)
- **TaskManagerIO**: For task scheduling (version 1.4.3)
- **OneWire**: For communication with OneWire devices (used by DallasTemperature)
- **Wire**: For I2C communication (built-in)

## Development Setup

### Build Configuration
The project uses PlatformIO for build management with the following configuration:
```ini
[env:mkrwifi1010]
platform = atmelsam
board = mkrwifi1010
framework = arduino
monitor_speed = 9600
build_flags = -std=c++23
build_unflags = -std=gnu++11
lib_deps = 
	bogde/HX711@^0.7.5
	milesburton/DallasTemperature@^3.11.0
	br3ttb/PID@^1.2.1
	duinowitchery/hd44780@^1.3.2
	tcmenu/TaskManagerIO@^1.4.3
check_skip_packages = yes

[env:native]
platform = native
build_flags = -std=c++23
build_unflags = -std=gnu++11
lib_deps =
	google/googletest@^1.12.1

[env:test]
platform = native
build_flags = 
	-std=c++23
	-DUNIT_TEST
	-I test
	-I src
	-I include
	-I .pio/libdeps/test/DallasTemperature/src
	-I .pio/libdeps/test/OneWire
build_unflags = -std=gnu++11
lib_deps =
	google/googletest@^1.12.1
	bogde/HX711@^0.7.5
	milesburton/DallasTemperature@^3.11.0
	br3ttb/PID@^1.2.1
	duinowitchery/hd44780@^1.3.2
	tcmenu/TaskManagerIO@^1.4.3
	paulstoffregen/OneWire@^2.3.7
test_framework = googletest
test_build_src = true
build_src_filter = +<src/*.cpp> +<src/*.h> -<src/main.cpp>
```
A Python script (`scripts/generate_compile_commands.py`) is used as an extra script in the PlatformIO build process to generate the `compile_commands.json` file, which is used by static analysis tools.

### Project Structure
- **Docker**: A Dockerfile and associated scripts are included to provide a consistent development environment (`distiller-tools`).
- **src/**: Contains the main source code
  - **main.cpp**: Main program entry point and distillation process logic
  - **constants.h**: Global constants
  - **\*_controller.h**: Controller classes for different subsystems
  - **\*.h**: Hardware abstraction classes
- **include/**: Additional include files
- **lib/**: Project-specific libraries
- **test/**: Test files
  - **test_thermometer.cpp**: Unit tests for the Thermometer class
  - **test_relay.cpp**: Unit tests for the Relay class
  - **test_heater_controller.cpp**: Unit tests for the HeaterController class
  - **test_flow_controller.cpp**: Unit tests for the FlowController class
  - **README.md**: Documentation for the testing framework
  - **run_tests.bat**: Windows batch script to run all tests
  - **run_tests.sh**: Unix shell script to run all tests
- **compile_commands.json**: Compilation database used by static analysis tools.

### Development Workflow
1. Code is developed and compiled using PlatformIO
2. Unit tests are written and run using Google Test and Google Mock
3. The compiled firmware is uploaded to the Arduino MKR WiFi 1010
4. Serial monitor (9600 baud) can be used for debugging
5. Static analysis is performed using clang-tidy with the generated `compile_commands.json`

## Technical Constraints

### Hardware Limitations
- **Processing Power**: The ATSAMD21 microcontroller has limited processing power (48 MHz, ARM Cortex-M0+)
- **Memory**: Limited RAM (32 KB) and flash memory (256 KB)
- **I/O Pins**: Limited number of digital and analog I/O pins
- **Power Requirements**: The system requires external power for the heaters and relays

### Software Limitations
- **Real-time Constraints**: The system must respond to events in real-time
- **Non-preemptive Multitasking**: The Arduino framework does not support true multitasking
- **Limited Debugging**: Limited debugging capabilities compared to desktop environments

### Communication Constraints
- **I2C Bus**: Limited by the number of devices and bus speed
- **OneWire Bus**: Limited by the number of devices and bus speed
- **Serial Communication**: Limited by baud rate (9600)

## Dependencies

### External Libraries
- **HX711**: Provides an interface to the HX711 load cell amplifier
  - Used for weight measurement
  - Requires calibration for accurate readings
- **DallasTemperature**: Provides an interface to the DS18B20 temperature sensors
  - Depends on OneWire library
  - Supports multiple sensors on a single bus
- **PID**: Implements a PID controller
  - Used for flow rate control
  - Requires tuning for optimal performance
- **hd44780**: Provides an interface to the LCD display
  - Supports multiple display types
  - Requires I2C communication
- **TaskManagerIO**: Provides a task scheduling system
  - Used for non-blocking concurrent operations
  - Allows for time-based task execution
- **Google Test**: A testing framework for C++
  - Used for unit testing
  - Provides assertions and test fixtures
- **Google Mock**: A mocking framework for C++
  - Used for mocking dependencies in unit tests
  - Allows testing components in isolation
  - Has limitations when dealing with function pointers, requiring workarounds
  - Requires adapter/delegation pattern for mocking function pointers effectively

### Hardware Dependencies
- **Power Supply**: The system requires a stable power supply for the microcontroller and relays
- **Heaters**: The system controls three heaters with different power levels
- **Valves**: The system controls eight valves for different functions
- **Sensors**: The system depends on accurate readings from temperature and weight sensors

## Tool Usage Patterns

### PlatformIO
- Used for building and uploading firmware
- Manages library dependencies
- Provides a unified development environment
- Generates the `compile_commands.json` for static analysis tools
- Runs inside a Docker container for consistent development environment
- Accessed through a convenience script (`scripts/pio-tools.sh`):
  - `./scripts/pio-tools.sh format` for code formatting with clang-format
  - `./scripts/pio-tools.sh tidy` for static analysis with clang-tidy
  - `./scripts/pio-tools.sh test` for running unit tests
  - `./scripts/pio-tools.sh all` for running all commands

### Arduino Framework
- Provides hardware abstraction
- Simplifies I/O operations
- Offers a wide range of libraries

### Task Scheduling
- Tasks are scheduled at fixed intervals using TaskManagerIO
- Critical operations are performed in separate tasks
- Tasks are canceled and rescheduled as the distillation process progresses

### Sensor Reading
- Sensors are read at regular intervals (every 1 second)
- Readings are filtered using a median filter to reduce noise
- Filtered readings are used for decision-making

### Control Logic
- State-based control flow
- PID control for flow rate
- Simple on/off control for heaters
- Direct control for valves

### Static Analysis (Clang-Tidy)
- Used to identify code quality issues and potential bugs
- Relies on the `compile_commands.json` for accurate analysis
- Configured with a `.clang-tidy` file to specify checks and options

### Mocking Patterns for Function Pointers
- Adapter/delegation pattern used to work around Google Mock limitations with function pointers
- Abstract base class defines the interface with pure virtual methods
- Mock class inherits from the base class and implements the interface
- Function pointer methods delegate to mockable methods without function pointers
- This pattern maintains testability while working with C++23 and modern Google Mock
