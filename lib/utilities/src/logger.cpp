#include "../include/logger.h"

// Additional includes for production builds
#if !defined(NATIVE) && !defined(UNIT_TEST)
#include <Arduino.h>
#include <SD.h>
#endif

// Convert log level to string
const char *Logger::levelToString(LogLevel level) {
  switch (level) {
  case DEBUG_LEVEL:
    return "DEBUG";
  case INFO:
    return "INFO";
  case WARNING:
    return "WARNING";
  case ERROR:
    return "ERROR";
  case CRITICAL:
    return "CRITICAL";
  default:
    return "UNKNOWN";
  }
}

/**
 * Constructor
 * @param serialInterface Interface for serial communication
 * @param sdInterface Interface for SD card operations (nullptr to disable SD logging)
 */
Logger::Logger(ISerialInterface *serialInterface, ISDInterface *sdInterface)
  : serialInterface(serialInterface), sdInterface(sdInterface), sdEnabled(sdInterface != nullptr) {}

/**
 * Initialize the logger
 * @param level Minimum log level to record
 */
void Logger::begin(LogLevel level) {
  serialInterface->begin(9600);
  minLevel = level;

  // Initialize SD card if enabled
  if (sdEnabled && sdInterface) {
    if (sdInterface->begin(CHIP_SELECT_PIN)) {
      sdAvailable = true;
      
      #if defined(UNIT_TEST) || defined(NATIVE)
      // For test/native builds, use File directly
      logFile = sdInterface->open(logFileName, FILE_WRITE);
      if (logFile) {
        log(INFO, "Logging to SD card started");
      } else {
        log(ERROR, "Failed to open log file on SD card");
        sdAvailable = false;
      }
      #else
      // For production, use the implementation defined in the source file
      File logFile = sdInterface->open(logFileName, FILE_WRITE);
      if (logFile) {
        logFilePtr = (void*)(&logFile);
        log(INFO, "Logging to SD card started");
      } else {
        log(ERROR, "Failed to open log file on SD card");
        sdAvailable = false;
      }
      #endif
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
void Logger::log(LogLevel level, const char *format, ...) {
  if (level < minLevel)
    return;

  char message[MAX_LOG_LINE];
  va_list args;
  va_start(args, format);
  vsnprintf(message, sizeof(message), format, args);
  va_end(args);

  // Format: [TIME][LEVEL] Message
  char logLine[MAX_LOG_LINE];
  snprintf(logLine, sizeof(logLine), "[%lu][%s] %s", millis(), levelToString(level), message);

  // Output to Serial
  serialInterface->println(logLine);

  // Output to SD card if available
  if (sdEnabled && sdAvailable) {
    #if defined(UNIT_TEST) || defined(NATIVE)
    // For test/native builds, use File directly
    if (logFile) {
      logFile.println(logLine);
      logFile.flush();
    }
    #else
    // For production, use the File pointer
    if (logFilePtr) {
      File* filePtr = static_cast<File*>(logFilePtr);
      filePtr->println(logLine);
      filePtr->flush();
    }
    #endif
  }
}

/**
 * Log a message at DEBUG level
 */
void Logger::debug(const char *format, ...) {
  if (DEBUG_LEVEL < minLevel)
    return;

  char message[MAX_LOG_LINE];
  va_list args;
  va_start(args, format);
  vsnprintf(message, sizeof(message), format, args);
  va_end(args);

  log(DEBUG_LEVEL, message);
}

/**
 * Log a message at INFO level
 */
void Logger::info(const char *format, ...) {
  if (INFO < minLevel)
    return;

  char message[MAX_LOG_LINE];
  va_list args;
  va_start(args, format);
  vsnprintf(message, sizeof(message), format, args);
  va_end(args);

  log(INFO, message);
}

/**
 * Log a message at WARNING level
 */
void Logger::warning(const char *format, ...) {
  if (WARNING < minLevel)
    return;

  char message[MAX_LOG_LINE];
  va_list args;
  va_start(args, format);
  vsnprintf(message, sizeof(message), format, args);
  va_end(args);

  log(WARNING, message);
}

/**
 * Log a message at ERROR level
 */
void Logger::error(const char *format, ...) {
  if (ERROR < minLevel)
    return;

  char message[MAX_LOG_LINE];
  va_list args;
  va_start(args, format);
  vsnprintf(message, sizeof(message), format, args);
  va_end(args);

  log(ERROR, message);
}

/**
 * Log a message at CRITICAL level
 */
void Logger::critical(const char *format, ...) {
  if (CRITICAL < minLevel)
    return;

  char message[MAX_LOG_LINE];
  va_list args;
  va_start(args, format);
  vsnprintf(message, sizeof(message), format, args);
  va_end(args);

  log(CRITICAL, message);
}