#include <Arduino.h>
#include <TaskManagerIO.h>

#include "display_controller.h"
#include "distillation_state_manager.h"
#include "flow_controller.h"
#include "heater_controller.h"
#include "lcd.h"
#include "scale.h"
#include "thermometer.h"
#include "valve_controller.h"

// Creating Thermometer objects
Thermometer mashTunThermometer(1);
Thermometer bottomThermometer(2);
Thermometer nearTopThermometer(3);
Thermometer topThermometer(4);

// Creating Scale objects
Scale earlyForeshotsScale(5, 6);
Scale lateForeshotsScale(7, 8);
Scale headsScale(9, 10);
Scale heartsScale(11, 12);
Scale earlyTailsScale(13, 14);
Scale lateTailsScale(15, 16);

// Creating objects for HeaterController
Relay heaterRelay1(13);
Relay heaterRelay2(14);
Relay heaterRelay3(15);

// Creating objects for ValveController (8 valves)
Relay valveRelay1(16);  // earlyForeshotsValve
Relay valveRelay2(17);  // lateForeshotsValve
Relay valveRelay3(18);  // headsValve
Relay valveRelay4(19);  // heartsValve
Relay valveRelay5(20);  // earlyTailsValve
Relay valveRelay6(21);  // lateTailsValve
Relay valveRelay7(22);  // coolantValve
Relay valveRelay8(23);  // mainValve

// Creating object for DisplayController
Lcd lcd(20, 4, 3);

// Creating controllers
HeaterController heaterController(heaterRelay1, heaterRelay2, heaterRelay3);
ValveController valveController(valveRelay1, valveRelay2, valveRelay3, valveRelay4, valveRelay5, valveRelay6, valveRelay7, valveRelay8);
ThermometerController thermometerController(mashTunThermometer, bottomThermometer, nearTopThermometer, topThermometer);
ScaleController scaleController(earlyForeshotsScale, lateForeshotsScale, headsScale, heartsScale, earlyTailsScale, lateTailsScale);
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

void updateAllThermometers() {
  mashTunThermometer.updateTemperature();
  bottomThermometer.updateTemperature();
  nearTopThermometer.updateTemperature();
  topThermometer.updateTemperature();
}

void updateAllScales() {
  earlyForeshotsScale.updateWeight();
  lateForeshotsScale.updateWeight();
  headsScale.updateWeight();
  heartsScale.updateWeight();
  earlyTailsScale.updateWeight();
  lateTailsScale.updateWeight();
}

bool hasReachedVolume(float distillateVolume) {
  return scaleController.getWeight(DistillationStateManager::getInstance().getState()) / ALCOHOL_DENSITY >=
         distillateVolume;
}

bool isTemperatureStabilized() { return bottomThermometer.getTemperature() - topThermometer.getTemperature() < 2; }

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

void collectLateTails() {
  DistillationStateManager::getInstance().setState(LATE_TAILS);
  heaterController.setPower(2000);
  valveController.openCoolantValve();
  valveController.openDistillateValve(LATE_TAILS);
  flowController.setAndControlFlowRate(33.0);
  if (mashTunThermometer.getTemperature() > 97.5) {
    taskManager.cancelTask(collectLateTailsTaskId);
    finalizeDistillationTaskId = taskManager.scheduleFixedRate(1000, finalizeDistillation);
  }
}


void collectEarlyTails() {
  DistillationStateManager::getInstance().setState(EARLY_TAILS);
  heaterController.setPower(2000);
  valveController.openCoolantValve();
  valveController.openDistillateValve(EARLY_TAILS);
  flowController.setAndControlFlowRate(33.0);
  if (hasReachedVolume(700) && isTemperatureStabilized()) {
    taskManager.cancelTask(collectEarlyTailsTaskId);
    collectLateTailsTaskId = taskManager.scheduleFixedRate(1000, collectLateTails);
  }
}

void collectHearts() {
  DistillationStateManager::getInstance().setState(HEARTS);
  heaterController.setPower(2000);
  valveController.openCoolantValve();
  valveController.openDistillateValve(HEARTS);
  if (!hasReachedVolume(5000) || !nearTopThermometer.isSuddenTemperatureIncrease(0.1)) {
    if (isTemperatureStabilized()) {
      flowController.setAndControlFlowRate(33.0);
    } else {
      flowController.setAndControlFlowRate(10.0);
    }
  } else {
    taskManager.cancelTask(collectHeartsTaskId);
    collectEarlyTailsTaskId = taskManager.scheduleFixedRate(1000, collectEarlyTails);
  }
}

void collectHeads() {
  DistillationStateManager::getInstance().setState(HEADS);
  heaterController.setPower(2000);
  valveController.openCoolantValve();
  valveController.openDistillateValve(HEADS);
  flowController.setAndControlFlowRate(33.0);
  if (hasReachedVolume(900) && isTemperatureStabilized()) {
    taskManager.cancelTask(collectHeadsTaskId);
    collectHeartsTaskId = taskManager.scheduleFixedRate(1000, collectHearts);
  }
}

void collectLateForeshots() {
  DistillationStateManager::getInstance().setState(LATE_FORESHOTS);
  heaterController.setPower(2000);
  valveController.openCoolantValve();
  valveController.openDistillateValve(LATE_FORESHOTS);
  flowController.setAndControlFlowRate(33.0);
  if (hasReachedVolume(400) && isTemperatureStabilized()) {
    taskManager.cancelTask(collectLateForeshotsTaskId);
    collectHeadsTaskId = taskManager.scheduleFixedRate(1000, collectHeads);
  }
}

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

void waitForTemperatureStabilization() {
  DistillationStateManager::getInstance().setState(STABILIZING);
  heaterController.setPower(2000);

  if (isTemperatureStabilized()) {
    taskManager.cancelTask(waitForTemperatureStabilizationTaskId);
    collectEarlyForeshotsTaskId = taskManager.scheduleFixedRate(1000, collectEarlyForeshots);
  }
}

void heatUpMash() {
  float temperature = topThermometer.getTemperature();
  if (temperature < 40.0) {
    heaterController.setPower(6000);
  } else {
    taskManager.cancelTask(heatUpMashTaskId);
    waitForTemperatureStabilizationTaskId = taskManager.scheduleFixedRate(1000, waitForTemperatureStabilization);
  }
}

void setup() {
  taskManager.scheduleFixedRate(1000, updateAllThermometers);
  taskManager.scheduleFixedRate(1000, updateAllScales);
  heatUpMashTaskId = taskManager.scheduleFixedRate(1000, heatUpMash);
}

void loop() { taskManager.runLoop(); }
