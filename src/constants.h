#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "../include/TaskManagerIO.h"

// Density constants
const double ALCOHOL_DENSITY = 0.868; // Density of alcohol in g/ml.

// Pin constants for thermometers
const int MASH_TUN_THERMOMETER_PIN = 1;
const int BOTTOM_THERMOMETER_PIN = 2;
const int NEAR_TOP_THERMOMETER_PIN = 3;
const int TOP_THERMOMETER_PIN = 4;

// Pin constants for scales
const int EARLY_FORESHOTS_SCALE_DATA_PIN = 5;
const int EARLY_FORESHOTS_SCALE_CLOCK_PIN = 6;
const int LATE_FORESHOTS_SCALE_DATA_PIN = 7;
const int LATE_FORESHOTS_SCALE_CLOCK_PIN = 8;
const int HEADS_SCALE_DATA_PIN = 9;
const int HEADS_SCALE_CLOCK_PIN = 10;
const int HEARTS_SCALE_DATA_PIN = 11;
const int HEARTS_SCALE_CLOCK_PIN = 12;
const int EARLY_TAILS_SCALE_DATA_PIN = 13;
const int EARLY_TAILS_SCALE_CLOCK_PIN = 14;
const int LATE_TAILS_SCALE_DATA_PIN = 15;
const int LATE_TAILS_SCALE_CLOCK_PIN = 16;

// Pin constants for heater relays
const int HEATER_RELAY_1_PIN = 13;
const int HEATER_RELAY_2_PIN = 14;
const int HEATER_RELAY_3_PIN = 15;

// Pin constants for valve relays
const int EARLY_FORESHOTS_VALVE_PIN = 16;
const int LATE_FORESHOTS_VALVE_PIN = 17;
const int HEADS_VALVE_PIN = 18;
const int HEARTS_VALVE_PIN = 19;
const int EARLY_TAILS_VALVE_PIN = 20;
const int LATE_TAILS_VALVE_PIN = 21;
const int COOLANT_VALVE_PIN = 22;
const int MAIN_VALVE_PIN = 23;

// LCD constants
const int LCD_COLUMNS = 20;
const int LCD_ROWS = 4;
const int LCD_PIN = 3;

// Array size constants
const int READINGS_ARRAY_SIZE = 5;
const int READINGS_ARRAY_MIDDLE_INDEX = 2;

// Time constants
const unsigned long TEN_MINUTES_MS = 600000;

// Power constants
const int HEATER_POWER_LEVEL_1 = 1000;
const int HEATER_POWER_LEVEL_2 = 2000;
const int HEATER_POWER_LEVEL_3 = 3000;
const int HEATER_POWER_LEVEL_MAX = 6000;

// Volume constants
const float EARLY_FORESHOTS_VOLUME_ML = 200.0F;
const float LATE_FORESHOTS_VOLUME_ML = 400.0F;
const float HEADS_VOLUME_ML = 900.0F;
const float HEARTS_VOLUME_ML = 5000.0F;
const float EARLY_TAILS_VOLUME_ML = 700.0F;
const float LATE_TAILS_VOLUME_ML = 600.0F;

// Flow rate constants
const float HIGH_FLOW_RATE_ML_PER_MIN = 33.0F;
const float LOW_FLOW_RATE_ML_PER_MIN = 10.0F;

// Temperature constants
const float TEMPERATURE_STABILIZATION_THRESHOLD_C = 2.0F;
const float SUDDEN_TEMPERATURE_INCREASE_THRESHOLD_C = 0.1F;
const float MIN_TEMPERATURE_THRESHOLD_C = 40.0F;
const float TEMPERATURE_COMPARISON_TOLERANCE = 0.001F;

// Time conversion constants
const float MS_TO_MINUTES = 60000.0F;

// Test constants
const float TEST_TOLERANCE = 0.1F;
const int TEST_PID_KP = 2;
const int TEST_PID_KI = 5;
const int TEST_PID_KD = 1;
const int TEST_RETURN_VALUE = 42;

#endif // CONSTANTS_H
