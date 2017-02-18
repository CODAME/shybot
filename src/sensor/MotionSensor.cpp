#include <Arduino.h>
#include <Adafruit_MCP23017.h>

#include "MotionSensor.h"

#include "constants.h"

MotionSensor::MotionSensor(Adafruit_MCP23017 *myMcp, sensor_orientation myOrientation) {
  mcp = myMcp;
  orientation = myOrientation;
  pin = getPin();
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
