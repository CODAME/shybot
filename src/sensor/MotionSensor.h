#ifndef SB_MOTIONSENSOR
#define SB_MOTIONSENSOR

#include <Arduino.h>
#include <Adafruit_MCP23017.h>
#include "constants.h"

#define STARTUP_TIME 60000


class MotionSensor {
  public:
    struct Motion {
      sensor_orientation orientation;
      bool moving;
    };

    MotionSensor(Adafruit_MCP23017 *myMcp, sensor_orientation myOrientation);

    void attachInterrupts();
    void getMotion(Motion *motion);

    int pin;
    sensor_orientation orientation;

  private:
    Adafruit_MCP23017 *mcp;
    int getPin();

    //maps to sensor_orientation
    int mcp_pin[8] = { 6, -1, 4, -1, 2, -1, 0, -1 };

};

#endif
