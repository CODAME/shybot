#include <Arduino.h>
#include <Servo.h>
#include "helpers.h"
#include "constants.h"
#include "navigation/Navigator.h"

#define STEER_MAX 180
#define STEER_MIN 0
#define SPEED_TOLERANCE_KPH .5
#define POWER_INCREMENT 2

Navigator::Navigator(int drivePin, int steerPin) {
  drive.attach(drivePin);
  steer.attach(steerPin);
}

void Navigator::go(StoreEntry *entry) {
  currentEntry = entry;
  makeSuggestions();
  averageSuggestions();
  followSuggestion(&avgSuggestion);
}

void Navigator::backup(double heading) {
  DEBUG("backing up");
  if (currentEntry->proximity[SENSOR_ORIENTATION_E]->distance < 500 &&
      currentEntry->proximity[SENSOR_ORIENTATION_W]->distance < 500) {
    setSteer(CENTER);
  } else if (currentEntry->proximity[SENSOR_ORIENTATION_E]->distance < 500) {
    setSteer(LEFT);
  } else if (currentEntry->proximity[SENSOR_ORIENTATION_W]->distance < 500) {
    setSteer(RIGHT);
  } else {
    if(heading < 180) {
      setSteer(RIGHT);
    } else {
      setSteer(LEFT);
    }
  }
  setSpeed(7, DIR_REVERSE);
}

void Navigator::followSuggestion(Suggestion *suggestion) {
  if (suggestion->heading < 45 ) {
    setSteer(RIGHT);
    setSpeed(suggestion->speed, DIR_FORWARD);
  } else if (suggestion->heading < 110) {
    setSteer(SHARP_RIGHT);
    setSpeed(suggestion->speed, DIR_FORWARD);
  } else if (suggestion->heading < 250) {
    backup(suggestion->heading);
  } else if (suggestion->heading < 315) {
    setSteer(SHARP_LEFT);
    setSpeed(suggestion->speed, DIR_FORWARD);
  } else {
    setSteer(LEFT);
    setSpeed(suggestion->speed, DIR_FORWARD);
  }
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

void Navigator::makeSuggestions() {
  lenSuggestions = 0;
  Suggestion *cur = &suggestions[lenSuggestions++];
  //set first to prefer forward movement
  cur->heading = 0;
  cur->speed = 3;
  cur->weight = 10;
  for (int i=0; i<NUM_PROXIMITY; i++) {
    ProximitySensor::Proximity *prox = currentEntry->proximity[i];
    if (prox->distance == 0) { continue; }
    double weight = (double) 1 / pow(.001 *(prox->distance - 300), 2);
    //Serial.println(String("PROX ") + i + ": HEADING: " + prox->orientation + " WEIGHT: " + weight + " DISTANCE: " + prox->distance);
    int heading = (sensor_heading[prox->orientation] + 180) % 360;
    cur = &suggestions[lenSuggestions++];
    cur->weight = weight;
    cur->heading = heading;
    cur->speed = 10;
  }
  for (int i=0; i<NUM_MOTION; i++) {
    MotionSensor::Motion *motion = currentEntry->motion[i];
    double weight = motion->moving ? SEV_MOTION : 0;
    int heading = (sensor_heading[motion->orientation] + 180) % 360;
    cur = &suggestions[lenSuggestions++];
    cur->weight = weight;
    cur->heading = heading;
    cur->speed = 20;
  }
}

void Navigator::setSpeed(double goalKPH, direction direction) {
  DEBUG(currentEntry->rpm.kph());
  double diff = goalKPH - currentEntry->rpm.kph();
  double inc = constrain((diff / goalKPH) * POWER_INCREMENT, -POWER_INCREMENT, POWER_INCREMENT);
  if (abs(diff) > SPEED_TOLERANCE_KPH) {
    setPower(constrain(currentPower + inc, 0, 100), direction);
  }
}

void Navigator::setPower(double power, direction direction) {
  if (direction != currentDirection) {
    currentPower = 50;
    drive.write(90);
    delay(300);
    drive.write(currentDirection == DIR_FORWARD ? 82 : 105);
    delay(300);
  }
  DEBUG(String("POWER: ") + power + "DIR: " + direction);
  if (direction == DIR_FORWARD) {
    drive.write(map(power, 0, 100, 98, 125));
  } else {
    int mappedPower = map(power, 0, 100, 86, 50);
    drive.write(mappedPower);
  }
  currentDirection = direction;
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
