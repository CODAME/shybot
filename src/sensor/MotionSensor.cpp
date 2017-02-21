#include <Arduino.h>
#include <Adafruit_MCP23017.h>

#include "MotionSensor.h"

#include "constants.h"

int mcp_pin[8] = { 6, -1, 4, -1, 2, -1, 0, -1 };

MotionSensor::MotionSensor(Adafruit_MCP23017 *myMcp, sensor_orientation myOrientation) {
  mcp = myMcp;
  orientation = myOrientation;
  pin = getPin();
}

void MotionSensor::attachInterrupts() {
  mcp->setupInterruptPin(pin, RISING);
}

void MotionSensor::getMotion(Motion *motion) {
  motion->orientation = orientation;
  if(millis() > STARTUP_TIME) {
    motion->moving = (bool) mcp->digitalRead(getPin());
  } else {
    motion->moving = false;
  }
};

int MotionSensor::getPin() {
  return mcp_pin[orientation];
}

int MotionSensor::getOrientationByPin(int pin) {
  for(int i=0; i<8; i++) {
    if(mcp_pin[i] == pin) {
      return i;
    }
  }

}
