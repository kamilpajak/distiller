#ifndef VALVE_CONTROLLER_H
#define VALVE_CONTROLLER_H

#include "distillation_state_manager.h"
#include "relay.h"
#ifndef UNIT_TEST
#include <Arduino.h>
#endif

/**
 * Class for managing valves.
 */
class ValveController {
private:
  Relay coolantValve;        /**< Relay for controlling the coolant valve. */
  Relay mainValve;           /**< Relay for controlling the main valve. */
  Relay earlyForeshotsValve; /**< Relay for controlling the early foreshots valve. */
  Relay lateForeshotsValve;  /**< Relay for controlling the late foreshots valve. */
  Relay headsValve;          /**< Relay for controlling the heads valve. */
  Relay heartsValve;         /**< Relay for controlling the hearts valve. */
  Relay earlyTailsValve;     /**< Relay for controlling the early tails valve. */
  Relay lateTailsValve;      /**< Relay for controlling the late tails valve. */

public:
  /**
   * Constructor for the ValveController class.
   * @param coolantValve Relay for controlling the coolant valve.
   * @param mainValve Relay for controlling the main valve.
   * @param earlyForeshotsValve Relay for controlling the early foreshots valve.
   * @param lateForeshotsValve Relay for controlling the late foreshots valve.
   * @param headsValve Relay for controlling the heads valve.
   * @param heartsValve Relay for controlling the hearts valve.
   * @param earlyTailsValve Relay for controlling the early tails valve.
   * @param lateTailsValve Relay for controlling the late tails valve.
   */
  ValveController(Relay coolantValve, Relay mainValve, Relay earlyForeshotsValve, Relay lateForeshotsValve,
                  Relay headsValve, Relay heartsValve, Relay earlyTailsValve, Relay lateTailsValve)
    : coolantValve(coolantValve), mainValve(mainValve), earlyForeshotsValve(earlyForeshotsValve),
      lateForeshotsValve(lateForeshotsValve), headsValve(headsValve), heartsValve(heartsValve),
      earlyTailsValve(earlyTailsValve), lateTailsValve(lateTailsValve) {}

  /**
   * Opens the distillate valve for the provided state and ensures all others are closed.
   * @param state The distillate state for which to open the valve.
   */
  void openDistillateValve(DistillationState state);

  /**
   * Closes all distillate valves.
   */
  void closeAllDistillateValves();

  /**
   * Opens the coolant valve.
   */
  void openCoolantValve();

  /**
   * Closes the coolant valve.
   */
  void closeCoolantValve();

  /**
   * Opens the main valve.
   */
  void openMainValve();

  /**
   * Closes the main valve.
   */
  void closeMainValve();
};

#endif // VALVE_CONTROLLER_H
