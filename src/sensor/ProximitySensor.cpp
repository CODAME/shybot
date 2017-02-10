#include "ProximitySensor.h"
#include "helpers.h"

int lastTrigger = millis();
int triggerPin = -1;

ProximitySensor::ProximitySensor(int sensorPin,
                                 sensor_orientation myOrientation
                                ) {
  pin = sensorPin;
  orientation = myOrientation;
};

ProximitySensor::Proximity* ProximitySensor::getProximity() {
  uint32_t analog = analogRead(pin);
  return new Proximity({ orientation, analog * 5 });
};

void ProximitySensor::setTriggerPin(int pin) {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
  triggerPin = pin;
}

void ProximitySensor::triggerIfNeeded() {
  if(triggerPin != -1 && (millis() - lastTrigger > SB_PROX_TRIGGER_INTERVAL_MS)) {
    DEBUG("will trigger");
    lastTrigger = millis();
    digitalWrite(triggerPin, HIGH);
    delayMicroseconds(SB_PROX_TRIGGER_LEN_US);
    delay(10);
    digitalWrite(triggerPin, LOW);
    delay(SB_PROX_TRIGGER_INTERVAL_MS);
  }
}
