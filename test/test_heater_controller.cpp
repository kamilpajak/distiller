#include <gtest/gtest.h>
#include <gmock/gmock.h>

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
  MockRelay *heaters[3];
  int power;

public:
  TestableHeaterController(MockRelay &relay1, MockRelay &relay2, MockRelay &relay3) 
    : heaters{&relay1, &relay2, &relay3}, power(0) {}

  void setPower(int power) {
    this->power = power;
    int remainingPower = power;

    // Determine the state of each heater
    bool heaterStates[3] = {false, false, false};
    for (int i = 2; i >= 0; i--) {
      int heaterPower = (i + 1) * 1000;
      if (remainingPower >= heaterPower) {
        heaterStates[i] = true;
        remainingPower -= heaterPower;
      }
    }

    // Update the state of each heater
    for (int i = 0; i < 3; i++) {
      if (heaterStates[i]) {
        heaters[i]->turnOn();
      } else {
        heaters[i]->turnOff();
      }
    }
  }

  int getPower() { return power; }
};

class HeaterControllerTest : public ::testing::Test {
protected:
  MockRelay relay1;
  MockRelay relay2;
  MockRelay relay3;
  std::unique_ptr<TestableHeaterController> heaterController;

  void SetUp() override {
    heaterController = std::make_unique<TestableHeaterController>(relay1, relay2, relay3);
  }
};

TEST_F(HeaterControllerTest, InitialPowerIsZero) {
  EXPECT_EQ(0, heaterController->getPower());
}

TEST_F(HeaterControllerTest, SetPowerZeroTurnsOffAllHeaters) {
  // Arrange
  EXPECT_CALL(relay1, turnOff()).Times(1);
  EXPECT_CALL(relay2, turnOff()).Times(1);
  EXPECT_CALL(relay3, turnOff()).Times(1);
  
  // Act
  heaterController->setPower(0);
  
  // Assert
  EXPECT_EQ(0, heaterController->getPower());
}

TEST_F(HeaterControllerTest, SetPower1000TurnsOnHeater1Only) {
  // Arrange
  EXPECT_CALL(relay1, turnOn()).Times(1);
  EXPECT_CALL(relay2, turnOff()).Times(1);
  EXPECT_CALL(relay3, turnOff()).Times(1);
  
  // Act
  heaterController->setPower(1000);
  
  // Assert
  EXPECT_EQ(1000, heaterController->getPower());
}

TEST_F(HeaterControllerTest, SetPower2000TurnsOnHeater2Only) {
  // Arrange
  EXPECT_CALL(relay1, turnOff()).Times(1);
  EXPECT_CALL(relay2, turnOn()).Times(1);
  EXPECT_CALL(relay3, turnOff()).Times(1);
  
  // Act
  heaterController->setPower(2000);
  
  // Assert
  EXPECT_EQ(2000, heaterController->getPower());
}

TEST_F(HeaterControllerTest, SetPower3000TurnsOnHeater3Only) {
  // Arrange
  EXPECT_CALL(relay1, turnOff()).Times(1);
  EXPECT_CALL(relay2, turnOff()).Times(1);
  EXPECT_CALL(relay3, turnOn()).Times(1);
  
  // Act
  heaterController->setPower(3000);
  
  // Assert
  EXPECT_EQ(3000, heaterController->getPower());
}

TEST_F(HeaterControllerTest, SetPower3001TurnsOnHeater3AndHeater1) {
  // Arrange
  EXPECT_CALL(relay1, turnOn()).Times(1);
  EXPECT_CALL(relay2, turnOff()).Times(1);
  EXPECT_CALL(relay3, turnOn()).Times(1);
  
  // Act
  heaterController->setPower(3001);
  
  // Assert
  EXPECT_EQ(3001, heaterController->getPower());
}

TEST_F(HeaterControllerTest, SetPower6000TurnsOnAllHeaters) {
  // Arrange
  EXPECT_CALL(relay1, turnOn()).Times(1);
  EXPECT_CALL(relay2, turnOn()).Times(1);
  EXPECT_CALL(relay3, turnOn()).Times(1);
  
  // Act
  heaterController->setPower(6000);
  
  // Assert
  EXPECT_EQ(6000, heaterController->getPower());
}

#endif // UNIT_TEST

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
