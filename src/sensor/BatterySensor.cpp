#include "sensor/BatterySensor.h"

BatterySensor::BatterySensor(int pin) {
  batteryPin = pin;
  pinMode(pin, INPUT);
}

void BatterySensor::getBattery(BatterySensor::Battery *battery) {
  double analog = analogRead(batteryPin);
  battery->volts = analog * ((double) MAX_VOLTS / 1024);
}
