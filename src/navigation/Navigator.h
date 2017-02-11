#ifndef SB_NAVIGATOR
#define SB_NAVIGATOR
#include <Arduino.h>
#include <Servo.h>

#include "store/StoreEntry.h"

#define NUM_PREV_STATES 8

enum {
  PROXIMITY_RANK = 1,
  RPM_RANK = 2
};

class Navigator {
  public:

    enum status {
      OK,
      HAZARD
    };

    enum turn {
      LEFT = 30,
      SHARP_LEFT = 0,
      CENTER = 90,
      RIGHT = 150,
      SHARP_RIGHT = 180
    };

    enum direction {
      DIR_FORWARD,
      DIR_REVERSE
    };

    Navigator(int drivePin, int steerPin);

/*
    state getState(StoreEntry *entry);
    */
    void go(StoreEntry *entry);
    void setSteer(turn turn);
    turn getSteer();
    void setSpeed(double goalKPH, direction direction, RPMSensor::RPM rpm);
    void setPower(double power, direction direction);
    double getPower();

  private:
    Servo drive;
    Servo steer;
    double currentPower = 0;
    direction currentDirection = DIR_FORWARD;
    turn currentTurn;
};

#endif
