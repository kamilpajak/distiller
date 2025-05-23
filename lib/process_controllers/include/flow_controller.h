#ifndef FLOW_CONTROLLER_H
#define FLOW_CONTROLLER_H

#include <PID_v1.h>
#include <cmath> // For std::abs
#include <constants.h>
#include <distillation_state_manager.h>
#include <scale_controller.h>
#include <valve_controller.h>

/**
 * Class for handling flow control.
 */
class FlowController {
private:
  ValveController *valveController;        /**< Pointer to ValveController object for controlling valves. */
  ScaleController *scaleController;        /**< Pointer to ScaleController object for controlling scales. */
  double input{0}, output{0}, setpoint{0}; /**< Variables for PID control. */
  PID pid;                                 /**< PID object for flow control. */
  double flowRate{0};                      /**< Flow rate in ml/min. */
  unsigned long startTime{0};              /**< Time at the start of the state. */
  double startVolume{0};                   /**< Alcohol volume at the start of the state. */

  /**
   * Returns the current volume of alcohol.
   */
  [[nodiscard]] double getCurrentVolume() {
    return scaleController->getWeight(DistillationStateManager::getInstance().getState()) / ALCOHOL_DENSITY;
  }

public:
  /**
   * Constructor for the FlowController class.
   * @param valveController Pointer to ValveController object for controlling valves.
   * @param scaleController Pointer to ScaleController object for controlling scales.
   */
  FlowController(ValveController *valveController, ScaleController *scaleController)
    : valveController(valveController), scaleController(scaleController),
      pid(&input, &output, &setpoint, TEST_PID_KP, TEST_PID_KI, TEST_PID_KD, DIRECT) {
    pid.SetMode(AUTOMATIC);
    valveController->closeMainValve();
  }

  /**
   * Returns the current flow rate.
   * @return The current flow rate in ml/min.
   */
  [[nodiscard]] double getFlowRate() const { return flowRate; }

  /**
   * Sets and controls the flow rate.
   *
   * This method first checks if the new flow rate is different from the current one.
   * If it is, it updates the flow rate, start volume, and start time.
   *
   * If the flow rate is zero, the main valve is closed and the method returns.
   *
   * If the flow rate is non-zero, the method controls the flow based on the PID output.
   *
   * @param newFlowRate The desired flow rate in ml/min.
   */
  void setAndControlFlowRate(double newFlowRate) {
    const double epsilon = 0.001;

    // Check if the new flow rate is significantly different from the current one
    if (std::abs(newFlowRate - flowRate) > epsilon) {
      flowRate = newFlowRate;
      startVolume = getCurrentVolume();
      startTime = millis();
    }

    if (flowRate == 0) {
      valveController->closeMainValve();
      return;
    }

    unsigned long currentMillis = millis();
    double elapsedTimeInMinutes = static_cast<double>(currentMillis - startTime) / MS_TO_MINUTES;
    double expectedVolume = flowRate * elapsedTimeInMinutes;
    double currentVolume = getCurrentVolume();
    input = expectedVolume - (currentVolume - startVolume);
    PID::Compute();

    double tolerance = TEST_TOLERANCE;
    if (output > tolerance) {
      valveController->openMainValve();
    } else if (output < -tolerance) {
      valveController->closeMainValve();
    } else {
      // If the output is close to 0 (within the tolerance), the valve remains in its current state
      // You can add additional logic here if you want to handle this situation differently
    }
  }
};

#endif // FLOW_CONTROLLER_H
