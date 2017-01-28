#include "RPMError.h"
#include "sensor/RPMSensor.h"
#include "navigation/Navigator.h"

Navigator::status RPMError::check(StoreEntry *entry, Navigator *navigator) {
  double rpm = entry->rpm.wheelRPM();
  if (rpm < MIN_RPM) {
    switch(navigator->getPower()) {
      case Navigator::REVERSE_HIGH:
        navigator->setPower(Navigator::STOP);
        break;
      case Navigator::REVERSE:
        navigator->setPower(Navigator::REVERSE_HIGH);
        break;
      case Navigator::STOP:
        return Navigator::OK;
      case Navigator::SLOW:
        navigator->setPower(Navigator::CRUISE);
        break;
      case Navigator::CRUISE:
        navigator->setPower(Navigator::RUN);
        break;
      case Navigator::RUN:
        navigator->setPower(Navigator::SPRINT);
        break;
      case Navigator::SPRINT:
        navigator->setPower(Navigator::STOP);
        break;
    };
    return Navigator::HAZARD;
  } else {
    return Navigator::OK;
  }
}
