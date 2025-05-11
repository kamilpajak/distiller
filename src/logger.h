#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>
#include <SD.h>
#include <stdarg.h>

/**
 * Logger class for the Distiller project.
 * Handles logging to Serial and optionally to SD card.
 */
class Logger {
public:
  // Log levels
  enum LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    CRITICAL
  };

private:
  static constexpr int MAX_LOG_LINE = 256;
  static constexpr int CHIP_SELECT_PIN = 4; // SD card CS pin, adjust as needed

  LogLevel minLevel = INFO;
  bool sdEnabled = false;
  bool sdAvailable = false;
  File logFile;
  const char* logFileName = "distiller.log";
  
  // Convert log level to string
  const char* levelToString(LogLevel level) {
    switch(level) {
      case DEBUG: return "DEBUG";
      case INFO: return "INFO";
      case WARNING: return "WARNING";
      case ERROR: return "ERROR";
      case CRITICAL: return "CRITICAL";
      default: return "UNKNOWN";
    }
  }

public:
  /**
   * Constructor
   * @param enableSD Whether to log to SD card
   */
  Logger(bool enableSD = false) : sdEnabled(enableSD) {}

  /**
   * Initialize the logger
   * @param level Minimum log level to record
   */
  void begin(LogLevel level = INFO) {
    Serial.begin(9600);
    minLevel = level;
    
    // Initialize SD card if enabled
    if (sdEnabled) {
      if (SD.begin(CHIP_SELECT_PIN)) {
        sdAvailable = true;
        logFile = SD.open(logFileName, FILE_WRITE);
        if (logFile) {
          log(INFO, "Logging to SD card started");
        } else {
          log(ERROR, "Failed to open log file on SD card");
          sdAvailable = false;
        }
      } else {
        log(ERROR, "SD card initialization failed");
        sdAvailable = false;
      }
    }
  }

  /**
   * Log a message with the given level
   * @param level Log level
   * @param format Format string (printf style)
   * @param ... Variable arguments
   */
  void log(LogLevel level, const char* format, ...) {
    if (level < minLevel) return;

    char message[MAX_LOG_LINE];
    va_list args;
    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);

    // Format: [TIME][LEVEL] Message
    char logLine[MAX_LOG_LINE];
    snprintf(logLine, sizeof(logLine), "[%lu][%s] %s", 
             millis(), levelToString(level), message);

    // Output to Serial
    Serial.println(logLine);
    
    // Output to SD card if available
    if (sdEnabled && sdAvailable && logFile) {
      logFile.println(logLine);
      logFile.flush();
    }
  }

  /**
   * Check if a log level is enabled
   * @param level Log level to check
   * @return True if the level is enabled, false otherwise
   */
  bool isLevelEnabled(LogLevel level) const {
    return level >= minLevel;
  }

  /**
   * Convenience methods for different log levels
   */
  void debug(const char* format, ...) {
    if (DEBUG < minLevel) return;

    char message[MAX_LOG_LINE];
    va_list args;
    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);

    log(DEBUG, message);
  }

  void info(const char* format, ...) {
    if (INFO < minLevel) return;

    char message[MAX_LOG_LINE];
    va_list args;
    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);

    log(INFO, message);
  }

  void warning(const char* format, ...) {
    if (WARNING < minLevel) return;

    char message[MAX_LOG_LINE];
    va_list args;
    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);

    log(WARNING, message);
  }

  void error(const char* format, ...) {
    if (ERROR < minLevel) return;

    char message[MAX_LOG_LINE];
    va_list args;
    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);

    log(ERROR, message);
  }

  void critical(const char* format, ...) {
    if (CRITICAL < minLevel) return;

    char message[MAX_LOG_LINE];
    va_list args;
    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);

    log(CRITICAL, message);
  }
};

#endif // LOGGER_H