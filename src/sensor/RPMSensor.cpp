#include <Arduino.h>
#include "RPMSensor.h"

#include "helpers.h"

//rotations is a count of rotations from start time
//dRotations counts rotations since the greater of
//1 second or the last call to getRPM

volatile int rotations = 0;
volatile uint16_t dRotations = 0;
int startTime = millis();

void onRotation() {
  rotations++;
  dRotations++;
}

RPMSensor::RPMSensor(int pin) {
  rpmPin = pin;
  attachInterrupt(digitalPinToInterrupt(rpmPin), &onRotation, FALLING);
}

RPMSensor::~RPMSensor() {
  detachInterrupt(digitalPinToInterrupt(rpmPin));
}

void RPMSensor::getRPM(RPMSensor::RPM *rpm) {
  double seconds = (millis() - startTime) / 1000;
  double currentRpm = (double)(dRotations * MINUTES_PER_HOUR) / seconds;
  if (seconds < 1) {
    rpm->rpm = lastRpm;
  } else {
    startTime = millis();
    dRotations = 0;
    rpm->rpm = currentRpm;
    lastRpm = currentRpm;
  }
  rpm->rotations = rotations;
}
