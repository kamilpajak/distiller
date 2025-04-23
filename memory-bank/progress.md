# Progress: Distiller

## What Works

The Distiller project has implemented the following components and functionality:

### Core Hardware Abstraction
- **Relay**: Basic relay control with on/off functionality
- **Thermometer**: Temperature sensing with DS18B20 sensors, including median filtering
- **Scale**: Weight measurement with HX711 load cells, including median filtering
- **LCD**: Display control with I2C LCD, including multiplexer support

### Controllers
- **HeaterController**: Controls heating elements with 3 power levels (1000W, 2000W, 3000W)
- **ValveController**: Controls valves for different fractions and coolant
- **ThermometerController**: Manages 4 temperature sensors
- **ScaleController**: Manages 6 weight sensors
- **FlowController**: Controls flow rates using PID control
- **DisplayController**: Manages the LCD display
- **DistillationStateManager**: Manages the state of the distillation process (singleton pattern)

### Distillation Process
- **State Management**: The system progresses through different states of the distillation process
- **Temperature Control**: The system controls heating elements based on temperature readings
- **Flow Control**: The system controls the flow rate using a PID controller
- **Fraction Collection**: The system directs different fractions to the appropriate collection containers

### Task Scheduling
- **Regular Sensor Updates**: Temperature and weight sensors are updated at regular intervals
- **State-Specific Tasks**: Each distillation state has its own task that runs at regular intervals

## What's Left to Build

### Testing and Validation
- **Unit Tests**: ✅ Developed unit tests for key components using Google Test and Google Mock
- **Integration Tests**: Test the interaction between different components
- **System Tests**: Test the entire system under different conditions
- **Calibration Procedures**: Develop procedures for calibrating sensors

### Feature Enhancements
- **Remote Monitoring**: Implement remote monitoring using the WiFi module
- **Data Logging**: Add data logging for analysis and optimization
- **User Interface**: Develop a user interface for configuration and monitoring
- **Configuration Options**: Add options for customizing the distillation process

### Documentation
- **Code Documentation**: Improve code comments and documentation
- **User Manual**: Create a user manual for operating the system
- **Setup Guide**: Develop a guide for setting up and configuring the system
- **Troubleshooting Guide**: Create a guide for troubleshooting common issues

### Optimizations
- **Memory Usage**: Optimize memory usage to reduce RAM requirements
- **Performance**: Improve performance for critical operations
- **Power Consumption**: Optimize power consumption for battery operation
- **Error Handling**: Enhance error detection and recovery mechanisms

## Current Status

The project is in a functional state with the core components implemented. The system can control the distillation process through all its phases, from heating the mash to collecting different fractions.

The code is organized in a modular way with clear separation of concerns, making it maintainable and extensible. The use of object-oriented design principles and design patterns (like the singleton pattern for state management) contributes to the code quality.

The system uses task scheduling for non-blocking concurrent operations, allowing it to perform multiple tasks simultaneously without blocking the main loop. This is crucial for real-time control of the distillation process.

Sensor readings are filtered using median filtering to reduce noise and improve measurement stability. This is important for accurate control of the distillation process.

The flow rate is controlled using a PID controller, which helps maintain a consistent flow rate despite variations in pressure and temperature. This is crucial for the quality of the distilled product.

### Recent Improvements

1. **C++23 Standard**: The project now uses the C++23 standard for both production and test code, allowing the use of modern C++ features.

2. **Testing Framework**: The project now includes Google Test and Google Mock for unit testing, providing a robust framework for testing the system's components in isolation.

3. **Unit Tests**: Several unit tests have been implemented for key components:
   - **Thermometer**: Tests for temperature reading, median filtering, and sudden temperature increase detection
   - **Relay**: Tests for relay control and state management
   - **HeaterController**: Tests for power level control and heater activation logic
   - **FlowController**: Tests for flow rate control and valve management

4. **Test Runners**: Added batch and shell scripts to easily run all tests.

## Known Issues

No specific issues have been identified yet, as the project is still in its initial state. However, potential areas of concern include:

1. **Sensor Calibration**: The accuracy of temperature and weight sensors depends on proper calibration.
2. **PID Tuning**: The PID controller for flow rate control may need tuning for optimal performance.
3. **Memory Usage**: The Arduino MKR WiFi 1010 has limited RAM (32 KB), which may be a constraint for future enhancements.
4. **Power Requirements**: The system requires external power for the heaters and relays, which may be a limitation in some environments.
5. **Error Handling**: The current implementation may not handle all possible error conditions gracefully.

## Evolution of Project Decisions

The project has made several key decisions that have shaped its development:

1. **Modular Architecture**: The decision to use a modular architecture with clear separation of concerns has made the code more maintainable and extensible.

2. **State-Based Control**: The decision to manage the distillation process through a series of states has simplified the control logic and made it more modular.

3. **Task Scheduling**: The decision to use task scheduling for non-blocking concurrent operations has improved the system's responsiveness and ability to handle multiple tasks.

4. **Sensor Filtering**: The decision to use median filtering for sensor readings has improved the stability and reliability of the system.

5. **PID Control**: The decision to use a PID controller for flow rate control has improved the precision and consistency of the distillation process.

6. **C++23 Standard**: The decision to use the C++23 standard allows the use of modern C++ features, improving code quality and maintainability.

7. **Test-Driven Development**: The decision to implement unit tests using Google Test and Google Mock enables test-driven development, improving code quality and reliability.

These decisions have contributed to a solid foundation for the project, setting it up for future enhancements and optimizations.
