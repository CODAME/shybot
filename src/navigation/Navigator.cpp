#include <Arduino.h>
#include <Servo.h>
#include "helpers.h"
#include "navigation/Navigator.h"
#include "navigation/error/ProximityError.h"
#include "navigation/error/RPMError.h"

#define STEER_MAX 180
#define STEER_MIN 0


Navigator::Navigator(int drivePin, int steerPin) {
  drive.attach(drivePin);
  steer.attach(steerPin);
}

void Navigator::go(StoreEntry *storeEntry) {
  int status = ProximityError::check(storeEntry, this) ||
    RPMError::check(storeEntry, this);
  if (status == OK) {
    setPower(SLOW);
  }
}

void Navigator::setPower(power power) {
  currentPower = power;
  drive.write(power);
}

Navigator::power Navigator::getPower() {
  return currentPower;
}

void Navigator::setSteer(turn turn) {
  currentTurn = turn;
  steer.write(turn);
}

Navigator::turn Navigator::getSteer() {
  return currentTurn;
}
