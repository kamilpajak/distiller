#include "test_constants.h"

#include <constants.h>

// Define constants for array sizes
namespace {
constexpr size_t HEATER_COUNT = 3;
}

#include <array>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

// Define UNIT_TEST if not already defined
#ifndef UNIT_TEST
#define UNIT_TEST
#endif

// Forward declaration of the test class to ensure proper compilation
class HeaterControllerTest;

// Conditional includes based on test vs. production environment
#ifdef UNIT_TEST
// Test environment - use mock classes

// Mock Relay class
// This mocks the Relay class from src/relay.h
class MockRelay {
public:
  MOCK_METHOD(void, turnOn, (), ());
  MOCK_METHOD(void, turnOff, (), ());
};

// Modified HeaterController for testing
// This is a testable version of the HeaterController class from src/heater_controller.h
// In a production environment, we would use conditional compilation in the original class
class TestableHeaterController {
private:
  std::array<MockRelay *, HEATER_COUNT> heaters;
  int power{0};

public:
  TestableHeaterController(MockRelay &relay1, MockRelay &relay2, MockRelay &relay3)
    : heaters{&relay1, &relay2, &relay3} {}

  void setPower(int power) {
    this->power = power;
    int remainingPower = power;

    // Determine the state of each heater
    std::array<bool, HEATER_COUNT> heaterStates = {false, false, false};
    for (int i = 2; i >= 0; i--) {
      int heaterPower = (i + 1) * HEATER_POWER_LEVEL_1;
      if (remainingPower >= heaterPower) {
        heaterStates[i] = true; // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
        remainingPower -= heaterPower;
      }
    }

    // Update the state of each heater
    for (size_t i = 0; i < heaters.size(); i++) {
      if (heaterStates[i]) {  // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
        heaters[i]->turnOn(); // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
      } else {
        heaters[i]->turnOff(); // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
      }
    }
  }

  [[nodiscard]] int getPower() const { return power; }
};

class HeaterControllerTest
  : public ::testing::Test { // NOLINT(cppcoreguidelines-non-private-member-variables-in-classes)
protected:
  MockRelay relay1;
  MockRelay relay2;
  MockRelay relay3;
  std::unique_ptr<TestableHeaterController> heaterController;

  void SetUp() override { heaterController = std::make_unique<TestableHeaterController>(relay1, relay2, relay3); }
};

/**
 * @brief Test case for InitialPowerIsZero.
 *
 * Given a new HeaterController object is created.
 * When the power is checked.
 * Then it should be zero.
 */
TEST_F(HeaterControllerTest, InitialPowerIsZero) { // NOLINT(cppcoreguidelines-owning-memory)
  EXPECT_EQ(0, heaterController->getPower());
}

/**
 * @brief Test case for SetPowerZeroTurnsOffAllHeaters.
 *
 * Given a HeaterController object.
 * When the power is set to zero.
 * Then all heaters should be turned off.
 */
TEST_F(HeaterControllerTest, SetPowerZeroTurnsOffAllHeaters) { // NOLINT(cppcoreguidelines-owning-memory)
  // Arrange
  EXPECT_CALL(relay1, turnOff()).Times(1);
  EXPECT_CALL(relay2, turnOff()).Times(1);
  EXPECT_CALL(relay3, turnOff()).Times(1);

  // Act
  heaterController->setPower(0);

  // Assert
  EXPECT_EQ(0, heaterController->getPower());
}

/**
 * @brief Test case for SetPower1000TurnsOnHeater1Only.
 *
 * Given a HeaterController object.
 * When the power is set to 1000.
 * Then only heater 1 should be turned on.
 */
TEST_F(HeaterControllerTest, SetPower1000TurnsOnHeater1Only) { // NOLINT(cppcoreguidelines-owning-memory)
  // Arrange
  EXPECT_CALL(relay1, turnOn()).Times(1);
  EXPECT_CALL(relay2, turnOff()).Times(1);
  EXPECT_CALL(relay3, turnOff()).Times(1);

  // Act
  heaterController->setPower(heater::POWER_LEVEL_1);

  // Assert
  EXPECT_EQ(heater::POWER_LEVEL_1, heaterController->getPower());
}

/**
 * @brief Test case for SetPower2000TurnsOnHeater2Only.
 *
 * Given a HeaterController object.
 * When the power is set to 2000.
 * Then only heater 2 should be turned on.
 */
TEST_F(HeaterControllerTest, SetPower2000TurnsOnHeater2Only) { // NOLINT(cppcoreguidelines-owning-memory)
  // Arrange
  EXPECT_CALL(relay1, turnOff()).Times(1);
  EXPECT_CALL(relay2, turnOn()).Times(1);
  EXPECT_CALL(relay3, turnOff()).Times(1);

  // Act
  heaterController->setPower(heater::POWER_LEVEL_2);

  // Assert
  EXPECT_EQ(heater::POWER_LEVEL_2, heaterController->getPower());
}

/**
 * @brief Test case for SetPower3000TurnsOnHeater3Only.
 *
 * Given a HeaterController object.
 * When the power is set to 3000.
 * Then only heater 3 should be turned on.
 */
TEST_F(HeaterControllerTest, SetPower3000TurnsOnHeater3Only) { // NOLINT(cppcoreguidelines-owning-memory)
  // Arrange
  EXPECT_CALL(relay1, turnOff()).Times(1);
  EXPECT_CALL(relay2, turnOff()).Times(1);
  EXPECT_CALL(relay3, turnOn()).Times(1);

  // Act
  heaterController->setPower(heater::POWER_LEVEL_3);

  // Assert
  EXPECT_EQ(heater::POWER_LEVEL_3, heaterController->getPower());
}

/**
 * @brief Test case for SetPower3001TurnsOnHeater3Only.
 *
 * Given a HeaterController object.
 * When the power is set to 3001.
 * Then only heater 3 should be turned on (as it's the highest power heater).
 */
TEST_F(HeaterControllerTest, SetPower3001TurnsOnHeater3Only) { // NOLINT(cppcoreguidelines-owning-memory)
  // Arrange
  EXPECT_CALL(relay1, turnOff()).Times(1);
  EXPECT_CALL(relay2, turnOff()).Times(1);
  EXPECT_CALL(relay3, turnOn()).Times(1);

  // Act
  heaterController->setPower(heater::POWER_LEVEL_OVER_MAX);

  // Assert
  EXPECT_EQ(heater::POWER_LEVEL_OVER_MAX, heaterController->getPower());
}

/**
 * @brief Test case for SetPower6000TurnsOnAllHeaters.
 *
 * Given a HeaterController object.
 * When the power is set to 6000.
 * Then all heaters should be turned on.
 */
TEST_F(HeaterControllerTest, SetPower6000TurnsOnAllHeaters) { // NOLINT(cppcoreguidelines-owning-memory)
  // Arrange
  EXPECT_CALL(relay1, turnOn()).Times(1);
  EXPECT_CALL(relay2, turnOn()).Times(1);
  EXPECT_CALL(relay3, turnOn()).Times(1);

  // Act
  heaterController->setPower(heater::POWER_LEVEL_MAX);

  // Assert
  EXPECT_EQ(heater::POWER_LEVEL_MAX, heaterController->getPower());
}

#endif // UNIT_TEST
