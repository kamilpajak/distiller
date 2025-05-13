#ifndef PID_V1_H
#define PID_V1_H

#include <cstdint>

#ifdef UNIT_TEST
// Mock PID class for unit tests
class PID {
public:
  PID(double *input, double *output, double *setpoint, double kp, double ki, double kd, int controllerDirection) {}
  void SetMode(int mode) {}
  static bool Compute() { return true; }
};

// Constants
constexpr int AUTOMATIC = 1;
constexpr int MANUAL = 0;
constexpr int DIRECT = 0;
constexpr int REVERSE = 1;
#else
// This would be the real PID class for production
class PID {
public:
  PID(double *input, double *output, double *setpoint, double kp, double ki, double kd, int controllerDirection) {}
  void SetMode(int mode) {}
  static bool Compute() { return true; }
};

// Constants
constexpr int AUTOMATIC = 1;
constexpr int MANUAL = 0;
constexpr int DIRECT = 0;
constexpr int REVERSE = 1;
#endif

#endif // PID_V1_H
