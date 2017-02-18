#ifndef SB_RPMSENSOR
#define SB_RPMSENSOR
#include <math.h>
#include "helpers.h"

#define WHEEL_DIAMETER_MM 150

#define MM_PER_KM 1000000
#define MINUTES_PER_HOUR 60

const double DIFF_RATIO = 40 / 16;
const double WHEEL_CIRCUMFERENCE_MM = WHEEL_DIAMETER_MM * M_PI;


class RPMSensor {
  public:
    struct RPM {
      double rpm;
      int rotations;
      double kph() {
        double mmPerMinute = wheelRPM() * WHEEL_CIRCUMFERENCE_MM;
        return mmPerMinute * MINUTES_PER_HOUR / MM_PER_KM;
      }
      double wheelRPM() {
        return rpm / DIFF_RATIO;
      };
    };
    RPMSensor(int rpmPin);
    ~RPMSensor();

    void getRPM(RPM *rpm);

  private:
    int rpmPin;
    int lastRpm = 0;

};

#endif
