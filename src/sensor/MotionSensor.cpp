#include <Adafruit_MCP23017.h>

#include "MotionSensor.h"

#include "constants.h"

MotionSensor::MotionSensor(Adafruit_MCP23017 *myMcp, sensor_orientation myOrientation) { mcp = myMcp;
  orientation = myOrientation;
}

bool MotionSensor::getMotion() {
  return mcp->digitalRead(getPin());
};

int MotionSensor::getPin() {
  return mcp_pin[orientation];
}
