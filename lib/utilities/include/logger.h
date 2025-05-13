#ifndef LOGGER_H
#define LOGGER_H

#include "hardware_interfaces.h"

#include <stdarg.h>

// For Arduino C functions
#if !defined(NATIVE) && !defined(UNIT_TEST)
#include <Arduino.h> // Include the real Arduino.h for production
#endif

/**
 * Logger class for the Distiller project.
 * Handles logging to Serial and optionally to SD card.
 */
class Logger {
public:
  // Log levels 
  // Rename DEBUG to DEBUG_LEVEL to avoid conflict with Arduino's DEBUG macro
  enum LogLevel { DEBUG_LEVEL, INFO, WARNING, ERROR, CRITICAL };

private:
  static constexpr int MAX_LOG_LINE = 256;
  
  // Chip select pin for SD card
  #ifndef CHIP_SELECT_PIN
  #define CHIP_SELECT_PIN 10
  #endif

  LogLevel minLevel = INFO;
  bool sdEnabled = false;
  bool sdAvailable = false;
  
  // Forward declare File to avoid dependencies in header
  #if defined(UNIT_TEST) || defined(NATIVE)
  File logFile;
  #else
  // For production, we'll handle the File in the implementation
  void* logFilePtr = nullptr; 
  #endif
  
  const char *logFileName = "distiller.log";

  // Hardware interfaces
  ISerialInterface *serialInterface;
  ISDInterface *sdInterface;

  // Convert log level to string - implementation moved to logger.cpp
  const char *levelToString(LogLevel level);

public:
  /**
   * Constructor
   * @param serialInterface Interface for serial communication
   * @param sdInterface Interface for SD card operations (nullptr to disable SD logging)
   */
  Logger(ISerialInterface *serialInterface, ISDInterface *sdInterface = nullptr);

  /**
   * Initialize the logger
   * @param level Minimum log level to record
   */
  void begin(LogLevel level = INFO);

  /**
   * Log a message with the given level
   * @param level Log level
   * @param format Format string (printf style)
   * @param ... Variable arguments
   */
  void log(LogLevel level, const char *format, ...);

  /**
   * Check if a log level is enabled
   * @param level Log level to check
   * @return True if the level is enabled, false otherwise
   */
  bool isLevelEnabled(LogLevel level) const { return level >= minLevel; }

  // Convenience methods for different log levels
  void debug(const char *format, ...);
  void info(const char *format, ...);
  void warning(const char *format, ...);
  void error(const char *format, ...);
  void critical(const char *format, ...);
};

#endif // LOGGER_H