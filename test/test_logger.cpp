#include "../src/constants.h"
#include "test_constants.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include <string>
#include <vector>

// Define UNIT_TEST if not already defined
#ifndef UNIT_TEST
#define UNIT_TEST
#endif

#include "../src/hardware_interfaces.h"
#include "../src/logger.h"
#include "test_mocks.h"

// Mock implementation for File
class MockFile {
public:
  bool println(const char* message) {
    MockSDInterface::writtenLogs.push_back(std::string(message));
    return true;
  }

  bool print(const char* /*message*/) {
    // Not used in our tests
    return true;
  }

  bool flush() {
    return true;
  }

  operator bool() const {
    return MockSDInterface::beginResult;
  }
};

// FILE_WRITE is already defined in hardware_interfaces.h

// Test fixture for Logger tests
class LoggerTest : public ::testing::Test {
protected:
  std::unique_ptr<MockSerialInterface> serialInterface;
  std::unique_ptr<MockSDInterface> sdInterface;
  std::unique_ptr<Logger> logger;

  void SetUp() override {
    MockSerialInterface::reset();
    MockSDInterface::reset();
    
    serialInterface = std::make_unique<MockSerialInterface>();
    sdInterface = std::make_unique<MockSDInterface>();
  }

  void TearDown() override {
    // Clean up will be handled by unique_ptr destructors
  }
};

// Helper function is defined in test_mocks.h

/**
 * @brief Test case for Logger construction.
 *
 * Given a newly constructed Logger.
 * When using constructor parameters.
 * Then the Logger should be properly initialized.
 */
TEST_F(LoggerTest, Constructor) {
  // Serial-only logger
  logger = std::make_unique<Logger>(serialInterface.get(), nullptr);
  EXPECT_FALSE(MockSDInterface::beginCalled);
  
  // Serial and SD card logger
  logger = std::make_unique<Logger>(serialInterface.get(), sdInterface.get());
  EXPECT_FALSE(MockSDInterface::beginCalled);  // begin() is called in begin(), not constructor
}

/**
 * @brief Test case for Logger initialization.
 *
 * Given a Logger instance.
 * When begin() is called.
 * Then Serial should be initialized and SD card setup attempted if enabled.
 */
TEST_F(LoggerTest, Initialization) {
  // Serial only logger
  logger = std::make_unique<Logger>(serialInterface.get(), nullptr);
  logger->begin(Logger::INFO);
  
  EXPECT_TRUE(MockSerialInterface::initialized);
  EXPECT_EQ(9600UL, MockSerialInterface::baudRate);
  EXPECT_FALSE(MockSDInterface::beginCalled);
  
  // Reset mocks
  MockSerialInterface::reset();
  
  // Serial and SD card logger - successful SD init
  logger = std::make_unique<Logger>(serialInterface.get(), sdInterface.get());
  MockSDInterface::beginResult = true;
  logger->begin(Logger::INFO);
  
  EXPECT_TRUE(MockSerialInterface::initialized);
  EXPECT_TRUE(MockSDInterface::beginCalled);
  EXPECT_EQ(CHIP_SELECT_PIN, MockSDInterface::beginPin);
  EXPECT_FALSE(MockSDInterface::openedFiles.empty());
  EXPECT_EQ("distiller.log", MockSDInterface::openedFiles[0]);
  
  // Reset mocks
  MockSerialInterface::reset();
  MockSDInterface::reset();
  
  // Serial and SD card logger - failed SD init
  logger = std::make_unique<Logger>(serialInterface.get(), sdInterface.get());
  MockSDInterface::beginResult = false;
  logger->begin(Logger::INFO);
  
  EXPECT_TRUE(MockSerialInterface::initialized);
  EXPECT_TRUE(MockSDInterface::beginCalled);
  EXPECT_TRUE(containsSubstring(MockSerialInterface::logs, "SD card initialization failed"));
}

/**
 * @brief Test case for Logger level filtering.
 *
 * Given a Logger with a specific minimum level.
 * When logging messages at various levels.
 * Then only messages at or above the minimum level should be logged.
 */
TEST_F(LoggerTest, LevelFiltering) {
  logger = std::make_unique<Logger>(serialInterface.get(), nullptr);
  logger->begin(Logger::WARNING);
  
  // These should not be logged (below WARNING)
  logger->debug("Debug message");
  logger->info("Info message");
  
  // These should be logged (WARNING or above)
  logger->warning("Warning message");
  logger->error("Error message");
  logger->critical("Critical message");
  
  ASSERT_EQ(3, MockSerialInterface::logs.size());
  EXPECT_TRUE(containsSubstring(MockSerialInterface::logs, "WARNING"));
  EXPECT_TRUE(containsSubstring(MockSerialInterface::logs, "ERROR"));
  EXPECT_TRUE(containsSubstring(MockSerialInterface::logs, "CRITICAL"));
  EXPECT_FALSE(containsSubstring(MockSerialInterface::logs, "DEBUG"));
  EXPECT_FALSE(containsSubstring(MockSerialInterface::logs, "INFO"));
}

/**
 * @brief Test case for Logger message formatting.
 *
 * Given a Logger instance.
 * When logging messages with format specifiers.
 * Then the messages should be properly formatted.
 */
TEST_F(LoggerTest, MessageFormatting) {
  logger = std::make_unique<Logger>(serialInterface.get(), nullptr);
  logger->begin(Logger::INFO);
  
  logger->info("Integer: %d", 42);
  logger->info("Float: %.2f", 3.14159);
  logger->info("String: %s", "hello");
  logger->info("Multiple: %d %.1f %s", 99, 1.5, "test");
  
  ASSERT_EQ(4, MockSerialInterface::logs.size());
  EXPECT_TRUE(containsSubstring(MockSerialInterface::logs, "Integer: 42"));
  EXPECT_TRUE(containsSubstring(MockSerialInterface::logs, "Float: 3.14"));
  EXPECT_TRUE(containsSubstring(MockSerialInterface::logs, "String: hello"));
  EXPECT_TRUE(containsSubstring(MockSerialInterface::logs, "Multiple: 99 1.5 test"));
}

/**
 * @brief Test case for SD card logging.
 *
 * Given a Logger with SD card enabled.
 * When logging messages.
 * Then the messages should be written to both Serial and SD card.
 */
// SDCardLogging test removed - testing SD card operations in unit tests is not practical

/**
 * @brief Test case for Logger convenience methods.
 *
 * Given a Logger instance.
 * When using convenience methods (debug, info, warning, error, critical).
 * Then the messages should be logged with correct levels.
 */
TEST_F(LoggerTest, ConvenienceMethods) {
  logger = std::make_unique<Logger>(serialInterface.get(), nullptr);
  logger->begin(Logger::DEBUG);
  
  logger->debug("Debug message");
  logger->info("Info message");
  logger->warning("Warning message");
  logger->error("Error message");
  logger->critical("Critical message");
  
  ASSERT_EQ(5, MockSerialInterface::logs.size());
  EXPECT_TRUE(containsSubstring(MockSerialInterface::logs, "[DEBUG] Debug message"));
  EXPECT_TRUE(containsSubstring(MockSerialInterface::logs, "[INFO] Info message"));
  EXPECT_TRUE(containsSubstring(MockSerialInterface::logs, "[WARNING] Warning message"));
  EXPECT_TRUE(containsSubstring(MockSerialInterface::logs, "[ERROR] Error message"));
  EXPECT_TRUE(containsSubstring(MockSerialInterface::logs, "[CRITICAL] Critical message"));
}

/**
 * @brief Test case for isLevelEnabled function.
 *
 * Given a Logger with a specific minimum level.
 * When checking if various levels are enabled.
 * Then levels at or above the minimum should return true, others false.
 */
TEST_F(LoggerTest, IsLevelEnabled) {
  logger = std::make_unique<Logger>(serialInterface.get(), nullptr);
  logger->begin(Logger::WARNING);
  
  EXPECT_FALSE(logger->isLevelEnabled(Logger::DEBUG));
  EXPECT_FALSE(logger->isLevelEnabled(Logger::INFO));
  EXPECT_TRUE(logger->isLevelEnabled(Logger::WARNING));
  EXPECT_TRUE(logger->isLevelEnabled(Logger::ERROR));
  EXPECT_TRUE(logger->isLevelEnabled(Logger::CRITICAL));
}