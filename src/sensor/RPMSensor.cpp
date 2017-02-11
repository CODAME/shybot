#include <Arduino.h>
#include "RPMSensor.h"

#include "helpers.h"

//rotations is a count of rotations from start time
//dRotations counts rotations since the greater of
//1 second or the last call to getRPM

volatile uint32_t rotations = 0;
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

RPMSensor::RPM RPMSensor::getRPM() {
  double seconds = (millis() - startTime) / 1000;
  double rpm = (dRotations * MINUTES_PER_HOUR) / seconds;
  if (seconds > .5) {
    startTime = millis();
    dRotations = 0;
  }
  return RPM({ rpm, rotations });
}
