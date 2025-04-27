#ifndef UNIT_TEST
#include "../include/Arduino.h"
#include "../include/TaskManagerIO.h"
#include "constants.h"
#include "display_controller.h"
#include "distillation_state_manager.h"
#include "flow_controller.h"
#include "heater_controller.h"
#include "lcd.h"
#include "scale_controller.h"
#include "thermometer_controller.h"
#include "valve_controller.h"

// Creating objects for thermometers
Thermometer mashTunThermometer(MASH_TUN_THERMOMETER_PIN);
Thermometer bottomThermometer(BOTTOM_THERMOMETER_PIN);
Thermometer nearTopThermometer(NEAR_TOP_THERMOMETER_PIN);
Thermometer topThermometer(TOP_THERMOMETER_PIN);

// Creating objects for scales
Scale earlyForeshotsScale(EARLY_FORESHOTS_SCALE_DATA_PIN, EARLY_FORESHOTS_SCALE_CLOCK_PIN);
Scale lateForeshotsScale(LATE_FORESHOTS_SCALE_DATA_PIN, LATE_FORESHOTS_SCALE_CLOCK_PIN);
Scale headsScale(HEADS_SCALE_DATA_PIN, HEADS_SCALE_CLOCK_PIN);
Scale heartsScale(HEARTS_SCALE_DATA_PIN, HEARTS_SCALE_CLOCK_PIN);
Scale earlyTailsScale(EARLY_TAILS_SCALE_DATA_PIN, EARLY_TAILS_SCALE_CLOCK_PIN);
Scale lateTailsScale(LATE_TAILS_SCALE_DATA_PIN, LATE_TAILS_SCALE_CLOCK_PIN);

// Creating objects for heater relays
Relay heaterRelay1(HEATER_RELAY_1_PIN);
Relay heaterRelay2(HEATER_RELAY_2_PIN);
Relay heaterRelay3(HEATER_RELAY_3_PIN);

// Creating objects for valve relays
Relay valveRelay1(EARLY_FORESHOTS_VALVE_PIN); // earlyForeshotsValve
Relay valveRelay2(LATE_FORESHOTS_VALVE_PIN);  // lateForeshotsValve
Relay valveRelay3(HEADS_VALVE_PIN);           // headsValve
Relay valveRelay4(HEARTS_VALVE_PIN);          // heartsValve
Relay valveRelay5(EARLY_TAILS_VALVE_PIN);     // earlyTailsValve
Relay valveRelay6(LATE_TAILS_VALVE_PIN);      // lateTailsValve
Relay valveRelay7(COOLANT_VALVE_PIN);         // coolantValve
Relay valveRelay8(MAIN_VALVE_PIN);            // mainValve

// Creating LCD object
Lcd lcd(LCD_COLUMNS, LCD_ROWS, LCD_PIN);

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
bool isTemperatureStabilized() {
  return bottomThermometer.getTemperature() - topThermometer.getTemperature() < TEMPERATURE_STABILIZATION_THRESHOLD_C;
}

// Finalize the distillation process
void finalizeDistillation() {
  DistillationStateManager::getInstance().setState(FINALIZING);
  static unsigned long startTime = 0;
  if (startTime == 0) {
    heaterController.setPower(0);
    startTime = millis();
  } else if (millis() - startTime >= TEN_MINUTES_MS) {
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
  heaterController.setPower(HEATER_POWER_LEVEL_2);
  valveController.openCoolantValve();
  valveController.openDistillateValve(LATE_TAILS);

  if (!hasReachedVolume(LATE_TAILS_VOLUME_ML)) {
    if (isTemperatureStabilized()) {
      flowController.setAndControlFlowRate(HIGH_FLOW_RATE_ML_PER_MIN); // Higher flow if stabilized
    } else {
      flowController.setAndControlFlowRate(LOW_FLOW_RATE_ML_PER_MIN); // Lower flow if not stabilized
    }
  } else {
    taskManager.cancelTask(collectLateTailsTaskId);
    finalizeDistillationTaskId = TaskManager::scheduleFixedRate(DEFAULT_TASK_RATE_MS, finalizeDistillation);
  }
}

// Collect early tails phase
void collectEarlyTails() {
  DistillationStateManager::getInstance().setState(EARLY_TAILS);
  heaterController.setPower(HEATER_POWER_LEVEL_2);
  valveController.openCoolantValve();
  valveController.openDistillateValve(EARLY_TAILS);

  if (!hasReachedVolume(EARLY_TAILS_VOLUME_ML)) {
    if (isTemperatureStabilized()) {
      flowController.setAndControlFlowRate(HIGH_FLOW_RATE_ML_PER_MIN); // Higher flow if stabilized
    } else {
      flowController.setAndControlFlowRate(LOW_FLOW_RATE_ML_PER_MIN); // Lower flow if not stabilized
    }
  } else {
    taskManager.cancelTask(collectEarlyTailsTaskId);
    collectLateTailsTaskId = TaskManager::scheduleFixedRate(DEFAULT_TASK_RATE_MS, collectLateTails);
  }
}

// Collect hearts phase
void collectHearts() {
  DistillationStateManager::getInstance().setState(HEARTS);
  heaterController.setPower(HEATER_POWER_LEVEL_2);
  valveController.openCoolantValve();
  valveController.openDistillateValve(HEARTS);

  if (!hasReachedVolume(HEARTS_VOLUME_ML) ||
      !nearTopThermometer.isSuddenTemperatureIncrease(SUDDEN_TEMPERATURE_INCREASE_THRESHOLD_C)) {
    if (isTemperatureStabilized()) {
      flowController.setAndControlFlowRate(HIGH_FLOW_RATE_ML_PER_MIN); // Higher flow if stabilized
    } else {
      flowController.setAndControlFlowRate(LOW_FLOW_RATE_ML_PER_MIN); // Lower flow if not stabilized
    }
  } else {
    taskManager.cancelTask(collectHeartsTaskId);
    collectEarlyTailsTaskId = TaskManager::scheduleFixedRate(DEFAULT_TASK_RATE_MS, collectEarlyTails);
  }
}

// Collect heads phase
void collectHeads() {
  DistillationStateManager::getInstance().setState(HEADS);
  heaterController.setPower(HEATER_POWER_LEVEL_2);
  valveController.openCoolantValve();
  valveController.openDistillateValve(HEADS);

  if (!hasReachedVolume(HEADS_VOLUME_ML)) {
    if (isTemperatureStabilized()) {
      flowController.setAndControlFlowRate(HIGH_FLOW_RATE_ML_PER_MIN); // Higher flow if stabilized
    } else {
      flowController.setAndControlFlowRate(LOW_FLOW_RATE_ML_PER_MIN); // Lower flow if not stabilized
    }
  } else {
    taskManager.cancelTask(collectHeadsTaskId);
    collectHeartsTaskId = TaskManager::scheduleFixedRate(DEFAULT_TASK_RATE_MS, collectHearts);
  }
}

// Collect late foreshots phase
void collectLateForeshots() {
  DistillationStateManager::getInstance().setState(LATE_FORESHOTS);
  heaterController.setPower(HEATER_POWER_LEVEL_2);
  valveController.openCoolantValve();
  valveController.openDistillateValve(LATE_FORESHOTS);

  if (!hasReachedVolume(LATE_FORESHOTS_VOLUME_ML)) {
    if (isTemperatureStabilized()) {
      flowController.setAndControlFlowRate(HIGH_FLOW_RATE_ML_PER_MIN); // Higher flow if stabilized
    } else {
      flowController.setAndControlFlowRate(LOW_FLOW_RATE_ML_PER_MIN); // Lower flow if not stabilized
    }
  } else {
    taskManager.cancelTask(collectLateForeshotsTaskId);
    collectHeadsTaskId = TaskManager::scheduleFixedRate(DEFAULT_TASK_RATE_MS, collectHeads);
  }
}

// Collect early foreshots phase
void collectEarlyForeshots() {
  DistillationStateManager::getInstance().setState(EARLY_FORESHOTS);
  heaterController.setPower(HEATER_POWER_LEVEL_2);
  valveController.openCoolantValve();
  valveController.openDistillateValve(EARLY_FORESHOTS);
  flowController.setAndControlFlowRate(LOW_FLOW_RATE_ML_PER_MIN);

  if (hasReachedVolume(EARLY_FORESHOTS_VOLUME_ML) && isTemperatureStabilized()) {
    taskManager.cancelTask(collectEarlyForeshotsTaskId);
    collectLateForeshotsTaskId = TaskManager::scheduleFixedRate(DEFAULT_TASK_RATE_MS, collectLateForeshots);
  }
}

// Wait for temperature stabilization phase
void waitForTemperatureStabilization() {
  DistillationStateManager::getInstance().setState(STABILIZING);
  heaterController.setPower(HEATER_POWER_LEVEL_2);

  if (isTemperatureStabilized()) {
    taskManager.cancelTask(waitForTemperatureStabilizationTaskId);
    collectEarlyForeshotsTaskId = TaskManager::scheduleFixedRate(DEFAULT_TASK_RATE_MS, collectEarlyForeshots);
  }
}

// Heat up mash phase
void heatUpMash() {
  float temperature = topThermometer.getTemperature();
  if (temperature < MIN_TEMPERATURE_THRESHOLD_C) {
    heaterController.setPower(HEATER_POWER_LEVEL_MAX);
  } else {
    taskManager.cancelTask(heatUpMashTaskId);
    waitForTemperatureStabilizationTaskId =
        TaskManager::scheduleFixedRate(DEFAULT_TASK_RATE_MS, waitForTemperatureStabilization);
  }
}

// Setup the process and schedule tasks
void setup() {
  TaskManager::scheduleFixedRate(DEFAULT_TASK_RATE_MS, [] {
    thermometerController.updateAllTemperatures();
    scaleController.updateAllWeights();
  });

  heatUpMashTaskId = TaskManager::scheduleFixedRate(DEFAULT_TASK_RATE_MS, heatUpMash);
}

// Main loop to manage tasks
void loop() { taskManager.runLoop(); }
#endif // UNIT_TEST
