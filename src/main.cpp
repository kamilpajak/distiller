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
Scale foreshotsScale(5, 6);
Scale headsScale(7, 8);
Scale heartsScale(9, 10);
Scale tailsScale(11, 12);

// Creating objects for HeaterController
Relay heaterRelay1(13);
Relay heaterRelay2(14);
Relay heaterRelay3(15);

// Creating objects for ValveController
Relay valveRelay1(16);
Relay valveRelay2(17);
Relay valveRelay3(18);
Relay valveRelay4(19);
Relay valveRelay5(20);
Relay valveRelay6(21);

// Creating object for DisplayController
Lcd lcd(20, 4, 3);

// Creating controllers
HeaterController heaterController(heaterRelay1, heaterRelay2, heaterRelay3);
ValveController valveController(valveRelay1, valveRelay2, valveRelay3, valveRelay4, valveRelay5, valveRelay6);
ThermometerController thermometerController(mashTunThermometer, bottomThermometer, nearTopThermometer, topThermometer);
ScaleController scaleController(foreshotsScale, headsScale, heartsScale, tailsScale);
FlowController flowController(&valveController, &scaleController);
DisplayController displayController(lcd, thermometerController, scaleController, flowController);

// Declare task identifiers
taskid_t heatUpMashTaskId;
taskid_t collectForeshotsTaskId;
taskid_t collectHeadsTaskId;
taskid_t collectHeartsTaskId;
taskid_t collectTailsTaskId;
taskid_t finalizeDistillationTaskId;

void updateAllThermometers() {
  mashTunThermometer.updateTemperature();
  bottomThermometer.updateTemperature();
  nearTopThermometer.updateTemperature();
  topThermometer.updateTemperature();
}

void updateAllScales() {
  foreshotsScale.updateWeight();
  headsScale.updateWeight();
  heartsScale.updateWeight();
  tailsScale.updateWeight();
}

bool hasReachedVolume(float distillateVolume) {
  return scaleController.getWeight(DistillationStateManager::getInstance().getState()) / ALCOHOL_DENSITY >=
         distillateVolume;
}

bool isTemperatureStabilized() { return bottomThermometer.getTemperature() - topThermometer.getTemperature() < 2; }

void finalizeDistillation() {
  DistillationStateManager::getInstance().setState(OFF);
  static unsigned long startTime = 0;
  if (startTime == 0) {
    heaterController.setPower(0);
    startTime = millis();
  } else if (millis() - startTime >= 120000) {
    valveController.closeCoolantValve();
    valveController.closeAllDistillateValves();
    flowController.setAndControlFlowRate(0.0);
    startTime = 0;
    taskManager.cancelTask(finalizeDistillationTaskId);
  }
}

void collectTails() {
  DistillationStateManager::getInstance().setState(TAILS);
  heaterController.setPower(2000);
  valveController.openCoolantValve();
  valveController.openDistillateValve(TAILS);
  flowController.setAndControlFlowRate(33.0);
  if (mashTunThermometer.getTemperature() > 97.5) {
    taskManager.cancelTask(collectTailsTaskId);
    finalizeDistillationTaskId = taskManager.scheduleFixedRate(1000, finalizeDistillation);
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
    collectTailsTaskId = taskManager.scheduleFixedRate(1000, collectTails);
  }
}

// collectMiddlings()

void collectHeads() {
  DistillationStateManager::getInstance().setState(HEADS);
  heaterController.setPower(2000);
  valveController.openCoolantValve();
  valveController.openDistillateValve(HEADS);
  flowController.setAndControlFlowRate(33.0);
  if (hasReachedVolume(700) && isTemperatureStabilized()) {
    taskManager.cancelTask(collectHeadsTaskId);
    collectHeartsTaskId = taskManager.scheduleFixedRate(1000, collectHearts);
  }
}

void collectForeshots() {
  DistillationStateManager::getInstance().setState(FORESHOTS);
  heaterController.setPower(2000);
  valveController.openCoolantValve();
  valveController.openDistillateValve(FORESHOTS);
  flowController.setAndControlFlowRate(10.0);
  if (hasReachedVolume(200) && isTemperatureStabilized()) {
    taskManager.cancelTask(collectForeshotsTaskId);
    collectHeadsTaskId = taskManager.scheduleFixedRate(1000, collectHeads);
  }
}

void heatUpMash() {
  float temperature = topThermometer.getTemperature();
  if (temperature < 40.0) {
    heaterController.setPower(6000);
  } else {
    taskManager.cancelTask(heatUpMashTaskId);
    collectForeshotsTaskId = taskManager.scheduleFixedRate(1000, collectForeshots);
  }
}

void setup() {
  taskManager.scheduleFixedRate(1000, updateAllThermometers);
  taskManager.scheduleFixedRate(1000, updateAllScales);
  heatUpMashTaskId = taskManager.scheduleFixedRate(1000, heatUpMash);
}

void loop() { taskManager.runLoop(); }
