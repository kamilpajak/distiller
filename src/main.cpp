#include "display_controller.h"
#include "distillation_state_manager.h"
#include "flow_controller.h"
#include "heater_controller.h"
#include "lcd.h"
#include "scale_controller.h"
#include "thermometer_controller.h"
#include "valve_controller.h"
#include <Arduino.h>
#include <TaskManagerIO.h>

// Creating objects for thermometers
Thermometer mashTunThermometer(1);
Thermometer bottomThermometer(2);
Thermometer nearTopThermometer(3);
Thermometer topThermometer(4);

// Creating objects for scales
Scale earlyForeshotsScale(5, 6);
Scale lateForeshotsScale(7, 8);
Scale headsScale(9, 10);
Scale heartsScale(11, 12);
Scale earlyTailsScale(13, 14);
Scale lateTailsScale(15, 16);

// Creating objects for heater relays
Relay heaterRelay1(13);
Relay heaterRelay2(14);
Relay heaterRelay3(15);

// Creating objects for valve relays
Relay valveRelay1(16); // earlyForeshotsValve
Relay valveRelay2(17); // lateForeshotsValve
Relay valveRelay3(18); // headsValve
Relay valveRelay4(19); // heartsValve
Relay valveRelay5(20); // earlyTailsValve
Relay valveRelay6(21); // lateTailsValve
Relay valveRelay7(22); // coolantValve
Relay valveRelay8(23); // mainValve

// Creating LCD object
Lcd lcd(20, 4, 3);

// Creating controllers
HeaterController heaterController(heaterRelay1, heaterRelay2, heaterRelay3);
ValveController valveController(valveRelay1, valveRelay2, valveRelay3, valveRelay4, valveRelay5, valveRelay6,
                                valveRelay7, valveRelay8);
ThermometerController thermometerController(mashTunThermometer, bottomThermometer, nearTopThermometer, topThermometer);
ScaleController scaleController(earlyForeshotsScale, lateForeshotsScale, headsScale, heartsScale, earlyTailsScale,
                                lateTailsScale);
FlowController flowController(&valveController, &scaleController);
DisplayController displayController(lcd, thermometerController, scaleController, flowController);

// Declare task identifiers
taskid_t heatUpMashTaskId;
taskid_t waitForTemperatureStabilizationTaskId;
taskid_t collectEarlyForeshotsTaskId;
taskid_t collectLateForeshotsTaskId;
taskid_t collectHeadsTaskId;
taskid_t collectHeartsTaskId;
taskid_t collectEarlyTailsTaskId;
taskid_t collectLateTailsTaskId;
taskid_t finalizeDistillationTaskId;

// Update all thermometers
void updateAllThermometers() { thermometerController.updateAllTemperatures(); }

// Update all scales
void updateAllScales() { scaleController.updateAllWeights(); }

// Check if the target volume is reached
bool hasReachedVolume(float distillateVolume) {
  return scaleController.getWeight(DistillationStateManager::getInstance().getState()) / ALCOHOL_DENSITY >=
         distillateVolume;
}

// Check if the temperature difference between bottom and top is small enough
bool isTemperatureStabilized() { return bottomThermometer.getTemperature() - topThermometer.getTemperature() < 2; }

// Finalize the distillation process
void finalizeDistillation() {
  DistillationStateManager::getInstance().setState(FINALIZING);
  static unsigned long startTime = 0;
  if (startTime == 0) {
    heaterController.setPower(0);
    startTime = millis();
  } else if (millis() - startTime >= 600000) {
    valveController.closeCoolantValve();
    valveController.closeAllDistillateValves();
    flowController.setAndControlFlowRate(0.0);
    startTime = 0;
    DistillationStateManager::getInstance().setState(OFF);
    taskManager.cancelTask(finalizeDistillationTaskId);
  }
}

// Collect late tails phase
void collectLateTails() {
  DistillationStateManager::getInstance().setState(LATE_TAILS);
  heaterController.setPower(2000);
  valveController.openCoolantValve();
  valveController.openDistillateValve(LATE_TAILS);

  if (!hasReachedVolume(600)) {
    if (isTemperatureStabilized()) {
      flowController.setAndControlFlowRate(33.0); // Higher flow if stabilized
    } else {
      flowController.setAndControlFlowRate(10.0); // Lower flow if not stabilized
    }
  } else {
    taskManager.cancelTask(collectLateTailsTaskId);
    finalizeDistillationTaskId = taskManager.scheduleFixedRate(1000, finalizeDistillation);
  }
}

// Collect early tails phase
void collectEarlyTails() {
  DistillationStateManager::getInstance().setState(EARLY_TAILS);
  heaterController.setPower(2000);
  valveController.openCoolantValve();
  valveController.openDistillateValve(EARLY_TAILS);

  if (!hasReachedVolume(700)) {
    if (isTemperatureStabilized()) {
      flowController.setAndControlFlowRate(33.0); // Higher flow if stabilized
    } else {
      flowController.setAndControlFlowRate(10.0); // Lower flow if not stabilized
    }
  } else {
    taskManager.cancelTask(collectEarlyTailsTaskId);
    collectLateTailsTaskId = taskManager.scheduleFixedRate(1000, collectLateTails);
  }
}

// Collect hearts phase
void collectHearts() {
  DistillationStateManager::getInstance().setState(HEARTS);
  heaterController.setPower(2000);
  valveController.openCoolantValve();
  valveController.openDistillateValve(HEARTS);

  if (!hasReachedVolume(5000) || !nearTopThermometer.isSuddenTemperatureIncrease(0.1)) {
    if (isTemperatureStabilized()) {
      flowController.setAndControlFlowRate(33.0); // Higher flow if stabilized
    } else {
      flowController.setAndControlFlowRate(10.0); // Lower flow if not stabilized
    }
  } else {
    taskManager.cancelTask(collectHeartsTaskId);
    collectEarlyTailsTaskId = taskManager.scheduleFixedRate(1000, collectEarlyTails);
  }
}

// Collect heads phase
void collectHeads() {
  DistillationStateManager::getInstance().setState(HEADS);
  heaterController.setPower(2000);
  valveController.openCoolantValve();
  valveController.openDistillateValve(HEADS);

  if (!hasReachedVolume(900)) {
    if (isTemperatureStabilized()) {
      flowController.setAndControlFlowRate(33.0); // Higher flow if stabilized
    } else {
      flowController.setAndControlFlowRate(10.0); // Lower flow if not stabilized
    }
  } else {
    taskManager.cancelTask(collectHeadsTaskId);
    collectHeartsTaskId = taskManager.scheduleFixedRate(1000, collectHearts);
  }
}

// Collect late foreshots phase
void collectLateForeshots() {
  DistillationStateManager::getInstance().setState(LATE_FORESHOTS);
  heaterController.setPower(2000);
  valveController.openCoolantValve();
  valveController.openDistillateValve(LATE_FORESHOTS);

  if (!hasReachedVolume(400)) {
    if (isTemperatureStabilized()) {
      flowController.setAndControlFlowRate(33.0); // Higher flow if stabilized
    } else {
      flowController.setAndControlFlowRate(10.0); // Lower flow if not stabilized
    }
  } else {
    taskManager.cancelTask(collectLateForeshotsTaskId);
    collectHeadsTaskId = taskManager.scheduleFixedRate(1000, collectHeads);
  }
}

// Collect early foreshots phase
void collectEarlyForeshots() {
  DistillationStateManager::getInstance().setState(EARLY_FORESHOTS);
  heaterController.setPower(2000);
  valveController.openCoolantValve();
  valveController.openDistillateValve(EARLY_FORESHOTS);
  flowController.setAndControlFlowRate(10.0);

  if (hasReachedVolume(200) && isTemperatureStabilized()) {
    taskManager.cancelTask(collectEarlyForeshotsTaskId);
    collectLateForeshotsTaskId = taskManager.scheduleFixedRate(1000, collectLateForeshots);
  }
}

// Wait for temperature stabilization phase
void waitForTemperatureStabilization() {
  DistillationStateManager::getInstance().setState(STABILIZING);
  heaterController.setPower(2000);

  if (isTemperatureStabilized()) {
    taskManager.cancelTask(waitForTemperatureStabilizationTaskId);
    collectEarlyForeshotsTaskId = taskManager.scheduleFixedRate(1000, collectEarlyForeshots);
  }
}

// Heat up mash phase
void heatUpMash() {
  float temperature = topThermometer.getTemperature();
  if (temperature < 40.0) {
    heaterController.setPower(6000);
  } else {
    taskManager.cancelTask(heatUpMashTaskId);
    waitForTemperatureStabilizationTaskId = taskManager.scheduleFixedRate(1000, waitForTemperatureStabilization);
  }
}

// Setup the process and schedule tasks
void setup() {
  taskManager.scheduleFixedRate(1000, [] {
    thermometerController.updateAllTemperatures();
    scaleController.updateAllWeights();
  });

  heatUpMashTaskId = taskManager.scheduleFixedRate(1000, heatUpMash);
}

// Main loop to manage tasks
void loop() { taskManager.runLoop(); }
