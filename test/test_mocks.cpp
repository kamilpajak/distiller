#include "test_mocks.h"

// Initialize static members for MockSerialInterface
std::vector<std::string> MockSerialInterface::logs;
bool MockSerialInterface::initialized = false;
unsigned long MockSerialInterface::baudRate = 0;

// Initialize static members for MockSDInterface
bool MockSDInterface::beginCalled = false;
bool MockSDInterface::beginResult = true;
int MockSDInterface::beginPin = 0;
std::vector<std::string> MockSDInterface::openedFiles;
std::vector<std::string> MockSDInterface::writtenLogs;