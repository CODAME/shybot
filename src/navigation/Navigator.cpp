#include <Arduino.h>
#include <Servo.h>
#include "helpers.h"
#include "navigation/navigator.h"
#include "navigation/error/ProximityError.h"

#define STEER_MAX 180
#define STEER_MIN 0


Navigator::Navigator(int drivePin, int steerPin) {
  drive.attach(drivePin);
  steer.attach(steerPin);
}

void Navigator::go(StoreEntry *storeEntry) {
  int status = ProximityError::check(storeEntry, this);
  if (status == OK) {
    setPower(SLOW);
  }
}

void Navigator::setPower(power power) {
  drive.write(power);
}

void Navigator::setSteer(int degrees) {
  steer.write(degrees);
}
