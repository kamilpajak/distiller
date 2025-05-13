#include "../include/valve_controller.h"

/**
 * Opens the distillate valve for the provided state and ensures all others are closed.
 * @param state The distillate state for which to open the valve.
 */
void ValveController::openDistillateValve(DistillationState state) {
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
void ValveController::closeAllDistillateValves() {
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
void ValveController::openCoolantValve() { coolantValve.turnOn(); }

/**
 * Closes the coolant valve.
 */
void ValveController::closeCoolantValve() { coolantValve.turnOff(); }

/**
 * Opens the main valve.
 */
void ValveController::openMainValve() { mainValve.turnOn(); }

/**
 * Closes the main valve.
 */
void ValveController::closeMainValve() { mainValve.turnOff(); }