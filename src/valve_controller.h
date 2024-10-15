#ifndef VALVE_CONTROLLER_H
#define VALVE_CONTROLLER_H

#include "relay.h"
#include "distillation_state_manager.h"
#include <Arduino.h>

/**
 * Class for managing valves.
 */
class ValveController {
private:
  Relay coolantValve;           /**< Relay for controlling the coolant valve. */
  Relay mainValve;              /**< Relay for controlling the main valve. */
  Relay earlyForeshotsValve;    /**< Relay for controlling the early foreshots valve. */
  Relay lateForeshotsValve;     /**< Relay for controlling the late foreshots valve. */
  Relay headsValve;             /**< Relay for controlling the heads valve. */
  Relay heartsValve;            /**< Relay for controlling the hearts valve. */
  Relay earlyTailsValve;        /**< Relay for controlling the early tails valve. */
  Relay lateTailsValve;         /**< Relay for controlling the late tails valve. */

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
  void openDistillateValve(DistillationState state) {
    closeAllDistillateValves();

    switch (state) {
    case EARLY_FORESHOTS:
      earlyForeshotsValve.turnOn();
      break;
    case LATE_FORESHOTS:
      lateForeshotsValve.turnOn();
      break;
    case HEADS:
      headsValve.turnOn();
      break;
    case HEARTS:
      heartsValve.turnOn();
      break;
    case EARLY_TAILS:
      earlyTailsValve.turnOn();
      break;
    case LATE_TAILS:
      lateTailsValve.turnOn();
      break;
    default:
      // Handle invalid state
      break;
    }
  }

  /**
   * Closes all distillate valves.
   */
  void closeAllDistillateValves() {
    earlyForeshotsValve.turnOff();
    lateForeshotsValve.turnOff();
    headsValve.turnOff();
    heartsValve.turnOff();
    earlyTailsValve.turnOff();
    lateTailsValve.turnOff();
  }

  /**
   * Opens the coolant valve.
   */
  void openCoolantValve() { coolantValve.turnOn(); }

  /**
   * Closes the coolant valve.
   */
  void closeCoolantValve() { coolantValve.turnOff(); }

  /**
   * Opens the main valve.
   */
  void openMainValve() { mainValve.turnOn(); }

  /**
   * Closes the main valve.
   */
  void closeMainValve() { mainValve.turnOff(); }
};

#endif // VALVE_CONTROLLER_H
