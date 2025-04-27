#ifndef DISPLAY_CONTROLLER_H
#define DISPLAY_CONTROLLER_H

#include "distillation_state_manager.h"
#include "flow_controller.h"
#include "lcd.h"
#include "scale_controller.h"
#include "thermometer_controller.h"

#include <array>
#include <iomanip>
#include <sstream>

// Time constants
constexpr unsigned long SECONDS_PER_HOUR = 3600;
constexpr unsigned long SECONDS_PER_MINUTE = 60;
constexpr int TIME_BUFFER_SIZE = 9; // HH:MM:SS + null terminator

class DisplayController {
private:
  Lcd &lcd;
  ThermometerController &thermometerController;
  ScaleController &scaleController;
  FlowController &flowController;

  /**
   * Returns the elapsed time since the start of distillation in the format HH:MM:SS.
   */
  static String getElapsedTimeFormatted() {
    unsigned long elapsedTime = DistillationStateManager::getInstance().getElapsedTime();
    unsigned long hours = elapsedTime / SECONDS_PER_HOUR;
    unsigned long minutes = (elapsedTime % SECONDS_PER_HOUR) / SECONDS_PER_MINUTE;
    unsigned long seconds = elapsedTime % SECONDS_PER_MINUTE;

    std::stringstream ss;
    ss << std::setfill('0') << std::setw(2) << hours << ":" << std::setfill('0') << std::setw(2) << minutes << ":"
       << std::setfill('0') << std::setw(2) << seconds;

    return {ss.str().c_str()};
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
