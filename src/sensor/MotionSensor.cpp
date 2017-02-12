#include <Adafruit_MCP23017.h>

#include "MotionSensor.h"

#include "constants.h"

MotionSensor::MotionSensor(Adafruit_MCP23017 *myMcp, sensor_orientation myOrientation) { mcp = myMcp;
  orientation = myOrientation;
}

void MotionSensor::getMotion(Motion *motion) {
  motion->orientation = orientation;
  motion->moving = (bool) mcp->digitalRead(getPin());
};

int MotionSensor::getPin() {
  return mcp_pin[orientation];
}
