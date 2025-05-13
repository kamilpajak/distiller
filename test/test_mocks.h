#pragma once

#include <cstdint>
#include <cstring>
#include <hardware_interfaces.h>
#include <string>
#include <vector>

// Mock for the Serial Interface - used by the Logger
class MockSerialInterface : public ISerialInterface {
public:
  static std::vector<std::string> logs;
  static bool initialized;
  static unsigned long baudRate;

  void begin(unsigned long baud) override {
    initialized = true;
    baudRate = baud;
    logs.clear();
  }

  size_t print(const char *str) override {
    size_t len = strlen(str);
    // Don't store in logs, as we're primarily testing println
    return len;
  }

  size_t println(const char *str) override {
    size_t len = strlen(str);
    logs.push_back(std::string(str));
    return len + 2; // +2 for \r\n
  }

  size_t print(float val, int format = 2) override {
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%.*f", format, val);
    return strlen(buffer);
  }

  size_t println(float val, int format = 2) override {
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%.*f", format, val);
    logs.push_back(std::string(buffer));
    return strlen(buffer) + 2; // +2 for \r\n
  }

  bool available() override {
    return false; // No incoming data in mock
  }

  static void reset() {
    logs.clear();
    initialized = false;
    baudRate = 0;
  }
};

// Mock for SD interface (we need it for test_logger.cpp)
class MockSDInterface : public ISDInterface {
public:
  static bool beginCalled;
  static bool beginResult;
  static int beginPin;
  static std::vector<std::string> openedFiles;
  static std::vector<std::string> writtenLogs;

  bool begin(uint8_t csPin = SS) override {
    beginCalled = true;
    beginPin = csPin;
    return beginResult;
  }

#if defined(UNIT_TEST) || defined(NATIVE)
  File open(const char *filename, const char *mode) override {
    openedFiles.push_back(std::string(filename));
    return File(); // Use our mock File class
  }
#else
  File open(const char *filename, uint8_t mode) override {
    openedFiles.push_back(std::string(filename));
    return File(); // Use our mock File class
  }
#endif

  bool exists(const char * /*filename*/) override { return beginResult; }

  bool mkdir(const char * /*filename*/) override { return beginResult; }

  static void reset() {
    beginCalled = false;
    beginResult = true;
    beginPin = 0;
    openedFiles.clear();
    writtenLogs.clear();
  }
};

// Helper function to check if a log message contains a substring
inline bool containsSubstring(const std::vector<std::string> &logs, const std::string &substring) {
  for (const auto &log : logs) {
    if (log.find(substring) != std::string::npos) {
      return true;
    }
  }
  return false;
}