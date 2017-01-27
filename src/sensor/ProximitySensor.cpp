#include <NewPing.h>

#include "ProximitySensor.h"
#include "helpers.h"

#define SONAR_MAX_DISTANCE 1000

ProximitySensor::ProximitySensor(int pin,
                                 sensor_orientation orientation,
                                 sensor_type type
                                ) {
  orientation = orientation;
  type = type;
  sonar = new NewPing(pin, pin, SONAR_MAX_DISTANCE);
};

ProximitySensor::Proximity* ProximitySensor::getProximity() {
  uint32_t distance = sonar->convert_cm(sonar->ping_median());
  DEBUG(distance);
  return new Proximity({ type, orientation, distance });
};
