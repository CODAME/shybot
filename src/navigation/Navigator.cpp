#include <Arduino.h>
#include <Servo.h>
#include "helpers.h"
#include "constants.h"
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

void Navigator::averageSuggestions() {
  double sumWeights = 0;
  Suggestion *cur;
  for(int i=0; i<lenSuggestions; i++) {
    if(suggestions[i].weight == 0 ) { continue; }
    cur = &suggestions[i];
    sumWeights += cur->weight;
    avgSuggestion.heading += cur->weight * cur->heading;
    avgSuggestion.speed += cur->weight * cur->speed;
  }

  avgSuggestion.heading /= sumWeights;
  avgSuggestion.speed /= sumWeights;
}

void Navigator::findSuggestions(StoreEntry *entry) {
  lenSuggestions = 0;
  Suggestion *cur;
  for (int i=0; i<NUM_PROXIMITY; i++) {
    ProximitySensor::Proximity *prox = entry->proximity[i];
    if (prox->distance == 0) { continue; }
    double weight = 1 + log((double)(SEV_PROXIMITY) / (double) (prox->distance - 300));
    int heading = (sensor_heading[prox->orientation] + 180) % 360;
    cur = &suggestions[lenSuggestions++];
    cur->weight = weight;
    cur->heading = heading;
    cur->speed = 10;
  }
  for (int i=0; i<NUM_MOTION; i++) {
    MotionSensor::Motion *motion = entry->motion[i];
    double weight = motion->moving ? SEV_MOTION : 0;
    int heading = (sensor_heading[motion->orientation] + 180) % 360;
    cur = &suggestions[lenSuggestions++];
    cur->weight = weight;
    cur->heading = heading;
    cur->speed = 80;
  }
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
