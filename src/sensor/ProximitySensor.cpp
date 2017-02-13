#include "ProximitySensor.h"
#include "helpers.h"

int lastTrigger = millis();
int triggerPin = -1;

ProximitySensor::ProximitySensor(MCP3008 *adc,
                                 sensor_orientation myOrientation
                                ) {
  _adc = adc;
  orientation = myOrientation;
};

void ProximitySensor::getProximity(Proximity *proximity) {
  uint32_t analog = _adc->readADC(getChannel());
  proximity->orientation = orientation;
  proximity->distance = analog * 5;
};

int ProximitySensor::getChannel() {
  return adc_pin[orientation];
}

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
