#ifndef SB_NAVIGATOR
#define SB_NAVIGATOR
#include <Arduino.h>
#include <Servo.h>
#include <LinkedList.h>

#include "store/StoreEntry.h"
#include "sensor/ProximitySensor.h"

#define MAX_THROTTLE 135
#define MIN_THROTTLE 50

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
      DIR_REVERSE,
      DIR_STOP
    };

    enum nav_mode {
      STOP,
      SCAN,
      RUN
    };

    struct Suggestion {
      double weight;
      int heading;
      int speed;
    };

    Navigator(int drivePin, int steerPin);

    void go(StoreEntry *entry);
    void backup(double heading = 0);
    void startBackup();
    void stopBackup();
    void run(int heading);
    void search();
    void safelyFollowHeading(int heading, int speed = 5);
    void followHeading(int heading, int speed = 5);
    int getMotionHeading();
    int getApproachHeading();
    ProximitySensor::Proximity* getMinProximity();
    ProximitySensor::Proximity* getMaxProximity();
    bool getDanger();
    bool getDanger(direction direction);
    double getAvgProximity();
    void setSteer(turn turn);
    turn getSteer();
    void setSpeed(double goalKPH, direction direction);
    void setPower(double power, direction direction);
    double getPower();
    void calibrate();

    Suggestion avgSuggestion = Suggestion({ 0, 0, 0});

  private:
    bool headingIsDanger(int heading, direction);

    Servo drive;
    Servo steer;
    StoreEntry *currentEntry;
    double currentPower = 0;
    direction currentDirection = DIR_FORWARD;
    turn currentTurn;
    float proximity_weight[NUM_PROXIMITY] = { 1, 1, .4, 0, 3, 0, .4, 1 };
    uint32_t timer = 0;
    uint32_t turnStart = 0;
    int backupGoal = 0;
    int backupHeading = 180;
    uint32_t lastDanger = 0;
    int initMotionHeading = 180;
    int mode = STOP;

};

#endif
