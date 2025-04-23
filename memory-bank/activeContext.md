# Active Context: Distiller

## Current Work Focus

The Distiller project is an automated distillation control system implemented on an Arduino MKR WiFi 1010 board. The system is designed to automate and optimize the distillation process, ensuring consistent quality and yield of the distilled product.

The current focus is on understanding the existing codebase and architecture to establish a solid foundation for future development and improvements.

## Recent Changes

The project is in its initial state with the following components implemented:

- Core hardware abstraction classes (Relay, Thermometer, Scale, LCD)
- Controller classes for different subsystems (Heater, Valve, Thermometer, Scale, Flow, Display)
- Distillation state management
- Main distillation process logic with different phases

## Next Steps

Potential next steps for the project include:

1. **Code Review and Optimization**:
   - Review the existing code for potential bugs or inefficiencies
   - Optimize memory usage and performance
   - Improve error handling and robustness

2. **Testing and Validation**:
   - ✅ Develop comprehensive tests for each component (implemented with Google Test and Google Mock)
   - Validate the system's behavior under different conditions
   - Ensure proper error handling and recovery

3. **Feature Enhancements**:
   - Add remote monitoring capabilities using the WiFi module
   - Implement data logging for analysis and optimization
   - Add user interface for configuration and monitoring

4. **Documentation**:
   - Create detailed documentation for the system
   - Document the distillation process and parameters
   - Create user manuals and setup guides

## Active Decisions and Considerations

### Architecture Decisions

1. **Modular Design**: The system is designed with a modular architecture, with clear separation of concerns between different components. This makes the code more maintainable and allows for easier testing and future enhancements.

2. **State-Based Control**: The distillation process is managed through a series of states, with each state having its own control function. This makes the code more modular and easier to maintain.

3. **Task Scheduling**: The system uses the TaskManagerIO library for non-blocking concurrent operations. This allows the system to perform multiple tasks simultaneously without blocking the main loop.

4. **C++23 Standard**: The project now uses the C++23 standard for both production and test code, allowing the use of modern C++ features.

5. **Testing Framework**: The project uses Google Test and Google Mock for unit testing, providing a robust framework for testing the system's components in isolation.

### Implementation Considerations

1. **Sensor Filtering**: Both temperature and weight sensors use median filtering to reduce noise and improve measurement stability. This is crucial for accurate control of the distillation process.

2. **PID Control**: The system uses a PID controller for flow rate control. The PID parameters may need tuning for optimal performance.

3. **Power Management**: The heater control is implemented with three relays of different power levels (1000W, 2000W, 3000W). This allows for more granular control of the heating power.

4. **Valve Control**: The system ensures that only one distillate valve is open at a time, based on the current distillation state. This prevents mixing of different fractions.

## Important Patterns and Preferences

### Code Organization

- **Header-Only Implementation**: The project uses header-only implementation for simplicity and to reduce compilation time.
- **Class-Based Design**: The system is organized into classes with clear responsibilities.
- **Singleton Pattern**: Used for the DistillationStateManager to ensure there is only one instance managing the state.
- **Test-Driven Development**: The project now includes comprehensive unit tests using Google Test and Google Mock, allowing for test-driven development.

### Naming Conventions

- **CamelCase for Classes**: All class names use CamelCase (e.g., `HeaterController`, `ValveController`).
- **camelCase for Methods and Variables**: Methods and variables use camelCase (e.g., `updateTemperature()`, `isTemperatureStabilized()`).
- **ALL_CAPS for Constants**: Constants use ALL_CAPS with underscores (e.g., `ALCOHOL_DENSITY`).

### Error Handling

- **Graceful Degradation**: The system should continue to operate even if some components fail.
- **Default Values**: The system uses default values when sensor readings are unavailable or invalid.

## Learnings and Project Insights

### Technical Insights

1. **Sensor Reliability**: Temperature and weight sensors are critical for the distillation process. Median filtering helps improve reliability, but calibration and proper installation are also important.

2. **Flow Control Challenges**: Controlling the flow rate accurately is challenging due to variations in pressure and temperature. The PID controller helps maintain a consistent flow rate, but tuning is crucial.

3. **State Transitions**: The transitions between different distillation states are critical points in the process. Clear conditions for state transitions help ensure proper operation.

### Process Insights

1. **Distillation Phases**: The distillation process is divided into distinct phases (foreshots, heads, hearts, tails), each with specific characteristics and requirements.

2. **Temperature Profiles**: The temperature profile during distillation provides valuable information about the process. Sudden temperature increases can indicate phase transitions.

3. **Volume Measurement**: Accurate measurement of the volume of each fraction is crucial for quality control. The system uses weight measurements and density conversion for this purpose.
