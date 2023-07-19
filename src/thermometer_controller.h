#ifndef THERMOMETER_CONTROLLER_H
#define THERMOMETER_CONTROLLER_H

#include "Thermometer.h"

class ThermometerController {
private:
  Thermometer &mashTunThermometer;
  Thermometer &bottomThermometer;
  Thermometer &nearTopThermometer;
  Thermometer &topThermometer;

public:
  ThermometerController(Thermometer &mashTunThermometer, Thermometer &bottomThermometer,
                        Thermometer &nearTopThermometer, Thermometer &topThermometer)
    : mashTunThermometer(mashTunThermometer), bottomThermometer(bottomThermometer),
      nearTopThermometer(nearTopThermometer), topThermometer(topThermometer) {}

  float getMashTunTemperature() { return mashTunThermometer.getTemperature(); }

  float getBottomTemperature() { return bottomThermometer.getTemperature(); }

  float getNearTopTemperature() { return nearTopThermometer.getTemperature(); }

  float getTopTemperature() { return topThermometer.getTemperature(); }
};

#endif // THERMOMETER_CONTROLLER_H
