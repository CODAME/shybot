#include <Arduino.h>
#include <Servo.h>
#include "helpers.h"
#include "constants.h"
#include "navigation/Navigator.h"

#define STEER_MAX 180
#define STEER_MIN 0
#define SPEED_TOLERANCE_KPH .5
#define POWER_INCREMENT 1

#define PIN_REVLIGHT 8

Navigator::Navigator(int drivePin, int steerPin) {
  drive.attach(drivePin);
  steer.attach(steerPin);
}

void Navigator::go(StoreEntry *entry) {
  currentEntry = entry;
  makeSuggestions();
  averageSuggestions();
  followSuggestion(&avgSuggestion);
  pinMode(PIN_REVLIGHT, OUTPUT);
}

void Navigator::backup(double heading) {
  DEBUG("backing up");
  digitalWrite(PIN_REVLIGHT, HIGH);
  if (currentEntry->proximity[SENSOR_ORIENTATION_E]->distance < 350 &&
      currentEntry->proximity[SENSOR_ORIENTATION_W]->distance < 350) {
    setSteer(CENTER);
  } else if (currentEntry->proximity[SENSOR_ORIENTATION_E]->distance < 350) {
    setSteer(LEFT);
  } else if (currentEntry->proximity[SENSOR_ORIENTATION_W]->distance < 350) {
    setSteer(RIGHT);
  } else {
    if(heading < 180) {
      setSteer(LEFT);
    } else {
      setSteer(RIGHT);
    }
  }
  setSpeed(7, DIR_REVERSE);
}

void Navigator::followSuggestion(Suggestion *suggestion) {
  digitalWrite(PIN_REVLIGHT, LOW);
  double heading = suggestion->heading;
  if (heading < 10 && heading > -10 ) {
    setSteer(CENTER);
    setSpeed(suggestion->speed, DIR_FORWARD);
  } else if (heading > 10 && heading < 30 ) {
    setSteer(RIGHT);
    setSpeed(suggestion->speed, DIR_FORWARD);
  } else if (heading < -10 && heading > -30 ) {
    setSteer(LEFT);
    setSpeed(suggestion->speed, DIR_FORWARD);
  } else if (heading > 10 && heading < 55){
    setSteer(RIGHT);
    setSpeed(suggestion->speed, DIR_FORWARD);
  } else if (heading < -10 && heading < -55) {
    setSteer(LEFT);
    setSpeed(suggestion->speed, DIR_FORWARD);
  } else {
    backup(suggestion->heading);
  }
}

void Navigator::averageSuggestions() {
  double sumWeights = 0;
  Suggestion *cur;
  double sumSin = 0;
  double sumCos = 0;
  for(int i=0; i<lenSuggestions; i++) {
    if(suggestions[i].weight == 0 ) { continue; }
    cur = &suggestions[i];
    sumWeights += cur->weight;
    sumSin += cur->weight * sin(cur->heading * M_PI / 180);
    sumCos += cur->weight * cos(cur->heading * M_PI / 180);
    avgSuggestion.speed += cur->weight * cur->speed;
  }

  avgSuggestion.heading  = (180 / M_PI) * atan2(sumSin / sumWeights, sumCos / sumWeights);
  avgSuggestion.speed /= sumWeights;
}

void Navigator::makeSuggestions() {
  lenSuggestions = 0;
  Suggestion *cur = &suggestions[lenSuggestions++];
  //set first to prefer forward movement
  cur->heading = 0;
  cur->speed = 5;
  cur->weight = 3;
  for (int i=0; i<NUM_PROXIMITY; i++) {
    ProximitySensor::Proximity *prox = currentEntry->proximity[i];
    if (prox->distance == 0) { continue; }
    double weight = proximity_weight[prox->orientation] * (double) SEV_PROXIMITY / (prox->distance - 250);
    int heading = sensor_heading[prox->orientation] + 180;
    Serial.println(String("HEADING ") + heading +  " WEIGHT: " + weight + " DISTANCE: " + prox->distance);
    cur = &suggestions[lenSuggestions++];
    cur->weight = weight;
    cur->heading = heading;
    cur->speed = 5;
  }
  if(millis() < 1000) { return; } //give PIRs time to warm up
  for (int i=0; i<NUM_MOTION; i++) {
    MotionSensor::Motion *motion = currentEntry->motion[i];
    double weight = motion->moving ? SEV_MOTION : 0;
    int heading = (sensor_heading[motion->orientation] + 180) ;
    Serial.println(String("HEADING ") + heading +  " WEIGHT: " + weight);
    cur = &suggestions[lenSuggestions++];
    cur->weight = weight;
    cur->heading = heading;
    cur->speed = 80;
  }
}

void Navigator::setSpeed(double goalKPH, direction direction) {
  if (direction != currentDirection) {
    DEBUG("SWITCHING");
    currentPower = 40;
    drive.write(90);
    delay(1000);
  }
  double diff = goalKPH - currentEntry->rpm.kph();
  double inc = constrain((diff / goalKPH) * POWER_INCREMENT, -POWER_INCREMENT, POWER_INCREMENT);
  if (abs(diff) > SPEED_TOLERANCE_KPH) {
    setPower(constrain(currentPower + inc, 0, 100), direction);
  }
}

void Navigator::setPower(double power, direction direction) {
  if (direction == DIR_FORWARD) {
    drive.write(map(power, 0, 100, 90, MAX_THROTTLE));
  } else {
    int mappedPower = map(power, 0, 100, 90, MIN_THROTTLE);
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

void Navigator::calibrate() {
    drive.write(MAX_THROTTLE);
    delay(2000);
    drive.write(MIN_THROTTLE);
    delay(2000);
    drive.write(90);
}

Navigator::turn Navigator::getSteer() {
  return currentTurn;
}
