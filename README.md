# Distiller

An automated distillation control system implemented on an Arduino MKR WiFi 1010 board. The system automates and optimizes the distillation process, ensuring consistent quality and yield of the distilled product.

## Overview

The Distiller project is designed to automate the distillation process, which traditionally requires constant manual monitoring and adjustment. By automating this process, we achieve:

- **Improved Consistency**: Ensures each batch follows the same precise parameters
- **Enhanced Efficiency**: Optimizes heating, cooling, and flow rates
- **Reduced Labor**: Eliminates the need for constant human supervision
- **Better Fraction Separation**: Provides more accurate separation of different fractions

## Features

- **Automated Process Control**: Fully automates the distillation process from heating to collection
- **Fraction Collection**: Automatically separates and collects different fractions (foreshots, heads, hearts, tails)
- **Temperature Monitoring**: Monitors temperatures at 4 points in the system
- **Weight Measurement**: Tracks the weight/volume of 6 collected fractions
- **Flow Control**: Manages flow rates using PID control for optimal distillation
- **Safety Features**: Prevents overheating and implements emergency shutdown procedures

## Hardware Requirements

- **Microcontroller**: Arduino MKR WiFi 1010 (ATSAMD21 + u-blox NINA-W10 WiFi module)
- **Temperature Sensors**: 4x DS18B20 digital temperature sensors
- **Weight Sensors**: 6x HX711 load cell amplifiers with load cells
- **Relays**: 11x Digital relay modules for controlling heaters and valves
- **Display**: I2C LCD display (20x4 characters)
- **I2C Multiplexer**: TCA9548A I2C multiplexer for managing multiple I2C devices
- **Power Supply**: External power for the heaters and relays

## Software Requirements

- **Development Platform**: PlatformIO
- **Framework**: Arduino
- **Target Platform**: Atmel SAM (atmelsam)
- **Libraries**:
  - HX711 (v0.7.5): For interfacing with load cell amplifiers
  - DallasTemperature (v3.11.0): For interfacing with DS18B20 temperature sensors
  - PID (v1.2.1): For implementing PID control
  - hd44780 (v1.3.2): For controlling the LCD display
  - TaskManagerIO (v1.4.3): For task scheduling
  - OneWire: For communication with OneWire devices
  - Wire: For I2C communication

## Installation

### Using PlatformIO

1. Clone the repository:
   ```
   git clone https://github.com/yourusername/distiller.git
   cd distiller
   ```

2. Build the project:
   ```
   pio run
   ```

3. Upload to Arduino MKR WiFi 1010:
   ```
   pio run -t upload
   ```

### Using Docker

1. Clone the repository:
   ```
   git clone https://github.com/yourusername/distiller.git
   cd distiller
   ```

2. Build the Docker image:
   ```
   docker build -t distiller-tools .
   ```

3. Run tests:
   ```bash
   docker run -v $(pwd):/project distiller-tools test
   ```

### Available Docker Commands

The Docker image includes scripts to run tests, code formatting, and code tidying. You can execute these using the `docker run` command with the appropriate argument:

- **Run tests**:
  ```bash
  docker run -v $(pwd):/project distiller-tools test
  ```
- **Run code formatting**:
  ```bash
  docker run -v $(pwd):/project distiller-tools format
  ```
- **Run code tidying/linting**:
  ```bash
  docker run -v $(pwd):/project distiller-tools tidy
  ```
- **Run all checks (format, tidy, test) sequentially**:
  ```bash
  docker run -v $(pwd):/project distiller-tools all
  ```

## Usage

The Distiller system operates automatically through the following phases:

1. **Heating Phase**: Heats the mash to the appropriate temperature (40°C)
2. **Stabilization Phase**: Waits for temperature stabilization
3. **Collection Phases**:
   - Early Foreshots (first 200ml)
   - Late Foreshots (next 400ml)
   - Heads (next 900ml)
   - Hearts (next 5000ml or until temperature increase detected)
   - Early Tails (next 700ml)
   - Late Tails (next 600ml)
4. **Finalization Phase**: Shuts down heating, continues cooling, then closes all valves

The LCD display shows the current status, temperatures, flow rates, and collected volumes.

## Project Structure

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

## Development

### Build Configuration

The project uses PlatformIO for build management with the following environments:

- **mkrwifi1010**: For building and uploading to the Arduino MKR WiFi 1010
- **native**: For building native code
- **test**: For running unit tests

### Testing

The project uses Google Test and Google Mock for unit testing. Tests are run using Docker:

```
docker run --rm -v $(pwd):/project distiller-tests /bin/bash -c "pio run -t clean && pio test -e test -v"
```

This command:
- Mounts the current directory to the /project directory in the container
- Cleans the project first
- Runs the tests with verbose output
- Removes the container after execution

## System Architecture

The Distiller system follows a modular architecture with clear separation of concerns:

### Core Components

- **Main Controller**: Orchestrates the overall distillation process
- **Distillation State Manager**: Manages the current state of the distillation process
- **Heater Controller**: Controls the heating elements (3 power levels)
- **Valve Controller**: Controls the valves for different fractions and coolant
- **Thermometer Controller**: Manages temperature sensors
- **Scale Controller**: Manages weight sensors
- **Flow Controller**: Controls flow rates using PID control
- **Display Controller**: Manages the LCD display

### Hardware Abstraction Layer

- **Relay**: Abstracts the control of digital output pins for heaters and valves
- **Thermometer**: Abstracts the DS18B20 temperature sensors
- **Scale**: Abstracts the HX711 load cell amplifiers
- **LCD**: Abstracts the I2C LCD display

## Current Status

The project is in a functional state with the core components implemented. The system can control the distillation process through all its phases, from heating the mash to collecting different fractions.

### What Works

- Core hardware abstraction
- Controllers for all subsystems
- Distillation process state management
- Temperature and flow control
- Fraction collection
- Task scheduling

### What's Left to Build

- Remote monitoring using WiFi
- Data logging for analysis
- User interface for configuration
- Additional configuration options
- Comprehensive documentation
- Performance optimizations

## License

[Specify your license here]
