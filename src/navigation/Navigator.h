#ifndef SB_NAVIGATOR
#define SB_NAVIGATOR
#include <Arduino.h>
#include <Servo.h>

#include "store/StoreEntry.h"
#include "sensor/ProximitySensor.h"

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
      SLEEP,
      RUN,
      COMM,
      OVERRIDE
    };

    struct Suggestion {
      double weight;
      int heading;
      int speed;
    };

    Navigator(int drivePin, int steerPin, int motorSwitch, StoreEntry *storeEntry);

    void go();
    void followOverride();
    void backup();
    void backup(int heading);
    void stop();
    void startRun();
    void startBackup(int heading);
    void stopBackup();
    void safelyFollowHeading(int heading, int speed = 5);
    void followHeading(int heading, int speed = 5);
    int getApproachHeading();
    ProximitySensor::Proximity* getMinProximity();
    ProximitySensor::Proximity* getMaxProximity();
    bool getDanger();
    bool getDanger(direction direction);
    int getBackupHeading();
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
    int drivePin;
    int steerPin;
    int motorSwitch;
    StoreEntry *currentEntry;
    double currentPower = 0;
    direction currentDirection = DIR_STOP;
    turn currentTurn;
    float proximity_weight[NUM_PROXIMITY] = { 1, 1, .4, 0, 3, 0, .4, 1 };
    uint32_t turnStart = 0;
    int lastIncrement = 0;
    int backupGoal = 0;
    int backupHeading = 0;
    int countBackups = 0;
    uint32_t lastDanger = 0;
    int stopTime = 0;
    int runStart = 0;
    int runStartTime = 0;
    int initMotionHeading = 180;

};

#endif
