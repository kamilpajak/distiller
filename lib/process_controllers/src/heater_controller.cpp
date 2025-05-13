#include "../include/heater_controller.h"

/**
 * Constructor for the HeaterController class.
 * @param relay1 Relay object for the first heater.
 * @param relay2 Relay object for the second heater.
 * @param relay3 Relay object for the third heater.
 */
HeaterController::HeaterController(Relay &relay1, Relay &relay2, Relay &relay3) : heaters{&relay1, &relay2, &relay3} {}

/**
 * Sets the power level of the heaters.
 * @param power The power level to set (0-6000).
 */
void HeaterController::setPower(int power) {
  this->power = power;
  int remainingPower = power;

  // Determine the state of each heater
  std::array<bool, 3> heaterStates = {false, false, false};
  for (int i = 2; i >= 0; i--) {
    int heaterPower = (i + 1) * HEATER_POWER_LEVEL_1;
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

/**
 * Returns the current power level of the heaters.
 * @return The current power level (0-6000).
 */
int HeaterController::getPower() const { return power; }