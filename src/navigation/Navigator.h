#ifndef SB_NAVIGATOR
#define SB_NAVIGATOR
#include <Arduino.h>
#include <Servo.h>

#include "store/StoreEntry.h"

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

    enum power {
      REVERSE_HIGH = 78,
      REVERSE = 82,
      STOP = 90,
      SLOW = 105,
      CRUISE = 110,
      RUN = 115,
      SPRINT = 125
    };

    Navigator(int drivePin, int steerPin);

    void go(StoreEntry *storeEntry);
    void setSteer(turn turn);
    turn getSteer();
    void setPower(power power);
    power getPower();

  private:
    Servo drive;
    Servo steer;
    power currentPower;
    turn currentTurn;
};

#endif
