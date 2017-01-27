#include <Arduino.h>
#include "RPMSensor.h"

#include "helpers.h"

volatile int rotations = 0;
int startTime = millis();

void onRotation() {
  rotations++;
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
  double rpm = (rotations * MINUTES_PER_HOUR) / seconds;
  if (seconds > 1) {
    startTime = 0;
    rotations = 0;
  }
  return RPM({ rpm });
}
