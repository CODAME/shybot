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
      volatile bool moving;
    };

    MotionSensor(Adafruit_MCP23017 *myMcp, sensor_orientation myOrientation);

    void attachInterrupts();
    void getMotion(Motion *motion);
    static int getOrientationByPin(int pin);

    int pin;
    sensor_orientation orientation;

  private:
    Adafruit_MCP23017 *mcp;
    int getPin();

    //maps to sensor_orientation

};

#endif
