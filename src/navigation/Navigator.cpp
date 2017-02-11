#include <Arduino.h>
#include <Servo.h>
#include "helpers.h"
#include "navigation/Navigator.h"

#define STEER_MAX 180
#define STEER_MIN 0
#define SPEED_TOLERANCE_KPH .5
#define POWER_INCREMENT 10


Navigator::Navigator(int drivePin, int steerPin) {
  drive.attach(drivePin);
  steer.attach(steerPin);
}

void Navigator::go(StoreEntry *entry) {
}

void Navigator::setSpeed(double goalKPH, direction direction, RPMSensor::RPM rpm) {
  double diff = goalKPH - rpm.kph();
  double inc = min((diff / goalKPH) * POWER_INCREMENT, POWER_INCREMENT);
  if (abs(diff) > SPEED_TOLERANCE_KPH) {
    setPower(currentPower + inc, direction);
  }
}

void Navigator::setPower(double power, direction direction) {
  if (direction != currentDirection) {
    drive.write(0);
    delay(300);
  }
  if (direction == DIR_FORWARD) {
    drive.write(map(power, 0, 100, 102, 125));
  } else {
    drive.write(map(power, 0, 100, 90, 78));
  }
  currentPower = power;
}

double Navigator::getPower() {
  return currentPower;
}

void Navigator::setSteer(turn turn) {
  currentTurn = turn;
  steer.write(turn);
}

Navigator::turn Navigator::getSteer() {
  return currentTurn;
}
