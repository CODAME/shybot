#ifndef SB_MOTIONSENSOR
#define SB_MOTIONSENSOR

#include <Arduino.h>
#include <Adafruit_MCP23017.h>
#include "constants.h"


class MotionSensor {
  public:
    MotionSensor(Adafruit_MCP23017 *myMcp, sensor_orientation myOrientation);

    bool getMotion();

    sensor_orientation orientation;

  private:
    Adafruit_MCP23017 *mcp;
    int getPin();

    //maps to sensor_orientation
    int mcp_pin[8] = { 0, -1, 2, -1, 4, -1, 6, -1 };

};

#endif
