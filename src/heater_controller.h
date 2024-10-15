#ifndef HEATER_CONTROLLER_H
#define HEATER_CONTROLLER_H

#include "relay.h"

/**
 * Class for controlling heaters.
 */
class HeaterController {
private:
  Relay *heaters[3]; /**< Array of pointers to Relay objects for controlling the heaters. */
  int power;         /**< Power level of the heaters. */

public:
  /**
   * Constructor for the HeaterController class.
   * @param relay1 Relay object for the first heater.
   * @param relay2 Relay object for the second heater.
   * @param relay3 Relay object for the third heater.
   */
  HeaterController(Relay &relay1, Relay &relay2, Relay &relay3) : heaters{&relay1, &relay2, &relay3}, power(0) {}

  /**
   * Sets the power level of the heaters.
   * @param power The power level to set (0-6000).
   */
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

  /**
   * Returns the current power level of the heaters.
   * @return The current power level (0-6000).
   */
  int getPower() { return power; }
};

#endif // HEATER_CONTROLLER_H
