#ifndef DISPLAY_CONTROLLER_H
#define DISPLAY_CONTROLLER_H

#include "distillation_state_manager.h"
#include "flow_controller.h"
#include "lcd.h"
#include "scale_controller.h"
#include "thermometer_controller.h"

class DisplayController {
private:
  Lcd &lcd;
  ThermometerController &thermometerController;
  ScaleController &scaleController;
  FlowController &flowController;

  /**
   * Returns the elapsed time since the start of distillation in the format HH:MM:SS.
   */
  String getElapsedTimeFormatted() {
    unsigned long elapsedTime = DistillationStateManager::getInstance().getElapsedTime();
    unsigned long hours = elapsedTime / 3600;
    unsigned long minutes = (elapsedTime % 3600) / 60;
    unsigned long seconds = elapsedTime % 60;

    char buffer[9];
    sprintf(buffer, "%02lu:%02lu:%02lu", hours, minutes, seconds);
    return String(buffer);
  }

public:
  DisplayController(Lcd &lcd, ThermometerController &thermometerController, ScaleController &scaleController,
                    FlowController &flowController)
    : lcd(lcd), thermometerController(thermometerController), scaleController(scaleController),
      flowController(flowController) {}

  void displayDistillationInfo() {
    lcd.clear();
    lcd.writeToRow("Elapsed: " + getElapsedTimeFormatted(), 0);
    lcd.writeToRow("State: " + String(DistillationStateManager::getInstance().getState()), 1);
    lcd.writeToRow("Flow: " + String(flowController.getFlowRate(), 0) + "ml/min", 2);
    lcd.writeToRow(
        "Volume: " +
            String(scaleController.getWeight(DistillationStateManager::getInstance().getState()) / ALCOHOL_DENSITY, 1) +
            "ml",
        3);
  }

  void displayTemperatureInfo() {
    lcd.clear();
    lcd.writeToRow("Top: " + String(thermometerController.getTopTemperature(), 1), 0);
    lcd.writeToRow("Middle: " + String(thermometerController.getNearTopTemperature(), 1), 1);
    lcd.writeToRow("Bottom: " + String(thermometerController.getBottomTemperature(), 1), 2);
    lcd.writeToRow("Mash tun: " + String(thermometerController.getMashTunTemperature(), 1), 3);
  }
};

#endif // DISPLAY_CONTROLLER_H
