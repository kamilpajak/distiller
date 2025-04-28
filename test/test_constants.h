#ifndef TEST_CONSTANTS_H
#define TEST_CONSTANTS_H

#include "../src/constants.h"

// Flow rate constants for tests
namespace flow {
    inline constexpr double DEFAULT_FLOW_RATE = 10.0;
    inline constexpr double ZERO_FLOW_RATE = 0.0;
    inline constexpr double FLOW_RATE_EPSILON = 0.001;
}

// Time constants for tests
namespace test_time {
    inline constexpr unsigned long ONE_MINUTE_MS = 60000;
}

// Weight constants for tests
namespace weight {
    inline constexpr double ZERO_WEIGHT = 0.0;
    inline const double EXPECTED_WEIGHT_10ML = 10.0 * ALCOHOL_DENSITY;
    inline const double EXPECTED_WEIGHT_20ML = 20.0 * ALCOHOL_DENSITY;
}

// PID output constants for tests
namespace pid {
    inline constexpr double POSITIVE_OUTPUT = 1.0;
    inline constexpr double NEGATIVE_OUTPUT = -1.0;
    inline constexpr double WITHIN_TOLERANCE_OUTPUT = 0.05;
}

// Heater power constants for tests
namespace heater {
    inline constexpr int POWER_LEVEL_1 = 1000;
    inline constexpr int POWER_LEVEL_2 = 2000;
    inline constexpr int POWER_LEVEL_3 = 3000;
    inline constexpr int POWER_LEVEL_MAX = 6000;
    inline constexpr int POWER_LEVEL_OVER_MAX = 3001;
}

// Temperature constants for tests
namespace temperature {
    inline constexpr float BASE = 20.0F;
    inline constexpr float SMALL_INCREMENT = 0.1F;
    inline constexpr float MEDIUM_INCREMENT = 1.0F;
    inline constexpr float LARGE_INCREMENT = 2.0F;
    inline constexpr float SUDDEN_INCREASE = 5.0F;
}

#endif // TEST_CONSTANTS_H
