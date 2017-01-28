#include "ProximityError.h"
#include "sensor/ProximitySensor.h"
#include "navigation/Navigator.h"

#define DISTANCE_THRESHOLD 80

Navigator::status ProximityError::check(StoreEntry *entry, Navigator *navigator) {
  ProximitySensor::Proximity *proximity;
  bool hazard = false;
  int distance;
  for (int i = 0; i < entry->numProximities(); i++) {
    distance = entry->proximities[i]->distance;
    DEBUG(distance);
    if(distance != 0 && distance <= DISTANCE_THRESHOLD) {
      hazard = true;
      proximity = entry->proximities[i];
      break;
    }
  }
  if (!hazard) {
    return Navigator::OK;
  }
  navigator->setPower(Navigator::REVERSE);
  delay(300);
  navigator->setPower(Navigator::STOP);
  delay(300);
  if (proximity->orientation == SENSOR_ORIENTATION_NE) {
    navigator->setSteer(Navigator::RIGHT);
  } else if (proximity->orientation == SENSOR_ORIENTATION_NW) {
    navigator->setSteer(Navigator::LEFT);
  }
  navigator->setPower(Navigator::REVERSE);
  delay(4000);
  navigator->setPower(Navigator::STOP);
  navigator->setSteer(Navigator::CENTER);

  return Navigator::HAZARD;
};
