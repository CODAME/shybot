#ifndef SB_RPMSENSOR
#define SB_RPMSENSOR
#include <math.h>
#include "helpers.h"

#define WHEEL_DIAMETER_MM 150

#define MINUTES_PER_HOUR 60

const double DIFF_RATIO = 40 / 16;
const double WHEEL_CIRCUMFERENCE = WHEEL_DIAMETER_MM * M_PI;


class RPMSensor {
  public:
    struct RPM {
      double rpm;
      double wheelRPM() {
        return rpm / DIFF_RATIO;
      };
    };
    RPMSensor(int rpmPin);
    ~RPMSensor();

    RPM getRPM();

  private:
    int rpmPin;

};

#endif
