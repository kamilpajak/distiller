#ifndef VALVE_CONTROLLER_H
#define VALVE_CONTROLLER_H

#include "Relay.h"
#include "distillation_state_manager.h"
#include <Arduino.h>

/**
 * Class for managing valves.
 */
class ValveController {
private:
  Relay coolantValve;   /**< Relay for controlling the coolant valve. */
  Relay mainValve;      /**< Relay for controlling the main valve. */
  Relay foreshotsValve; /**< Relay for controlling the foreshots valve. */
  Relay headsValve;     /**< Relay for controlling the heads valve. */
  Relay heartsValve;    /**< Relay for controlling the hearts valve. */
  Relay tailsValve;     /**< Relay for controlling the tails valve. */

public:
  /**
   * Constructor for the ValveController class.
   * @param coolantValve Relay for controlling the coolant valve.
   * @param mainValve Relay for controlling the main valve.
   * @param foreshotsValve Relay for controlling the foreshots valve.
   * @param headsValve Relay for controlling the heads valve.
   * @param heartsValve Relay for controlling the hearts valve.
   * @param tailsValve Relay for controlling the tails valve.
   */
  ValveController(Relay coolantValve, Relay mainValve, Relay foreshotsValve, Relay headsValve, Relay heartsValve,
                  Relay tailsValve)
    : coolantValve(coolantValve), mainValve(mainValve), foreshotsValve(foreshotsValve), headsValve(headsValve),
      heartsValve(heartsValve), tailsValve(tailsValve) {}

  /**
   * Opens the distillate valve for the provided state and ensures all others are closed.
   * @param state The distillate state for which to open the valve.
   */
  void openDistillateValve(DistillationState state) {
    closeAllDistillateValves();

    switch (state) {
    case FORESHOTS:
      foreshotsValve.turnOn();
      break;
    case HEADS:
      headsValve.turnOn();
      break;
    case HEARTS:
      heartsValve.turnOn();
      break;
    case TAILS:
      tailsValve.turnOn();
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
    foreshotsValve.turnOff();
    headsValve.turnOff();
    heartsValve.turnOff();
    tailsValve.turnOff();
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
