#ifndef HEATER_CONTROLLER_H
#define HEATER_CONTROLLER_H

#include "constants.h"
#include "relay.h"

#include <array>

/**
 * Class for controlling heaters.
 */
class HeaterController {
private:
  std::array<Relay *, 3> heaters; /**< Array of pointers to Relay objects for controlling the heaters. */
  int power{0};                   /**< Power level of the heaters. */

public:
  /**
   * Constructor for the HeaterController class.
   * @param relay1 Relay object for the first heater.
   * @param relay2 Relay object for the second heater.
   * @param relay3 Relay object for the third heater.
   */
  HeaterController(Relay &relay1, Relay &relay2, Relay &relay3);

  /**
   * Sets the power level of the heaters.
   * @param power The power level to set (0-6000).
   */
  void setPower(int power);

  /**
   * Returns the current power level of the heaters.
   * @return The current power level (0-6000).
   */
  [[nodiscard]] int getPower() const;
};

#endif // HEATER_CONTROLLER_H
