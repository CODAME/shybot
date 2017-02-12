#ifndef SB_NAVIGATOR
#define SB_NAVIGATOR
#include <Arduino.h>
#include <Servo.h>
#include <LinkedList.h>

#include "store/StoreEntry.h"

#define NUM_SUGGESTIONS NUM_PROXIMITY + NUM_MOTION

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

    enum hazard_type {
      OBJECT,
      HUMAN
    };

    enum severity {
      SEV_PROXIMITY = 1000,
      SEV_MOTION = 100
    };

    struct Hazard {
      double heading;
      int distance;
      hazard_type type;
      time_t timestamp;
    };

    struct Suggestion {
      double weight;
      int heading;
      int speed;
    };

    Navigator(int drivePin, int steerPin);

    void go(StoreEntry *entry);
    void findSuggestions(StoreEntry *entry);
    void averageSuggestions();
    void setSteer(turn turn);
    turn getSteer();
    void setSpeed(double goalKPH, direction direction, RPMSensor::RPM rpm);
    void setPower(double power, direction direction);
    double getPower();

    Suggestion avgSuggestion = Suggestion({ 0, 0, 0});

  private:
    Servo drive;
    Servo steer;
    double currentPower = 0;
    direction currentDirection = DIR_FORWARD;
    turn currentTurn;
    int lenSuggestions = 0;
    Suggestion suggestions[NUM_SUGGESTIONS] = {
      Suggestion({ 0, 0, 0}),
      Suggestion({ 0, 0, 0}),
      Suggestion({ 0, 0, 0}),
      Suggestion({ 0, 0, 0}),
      Suggestion({ 0, 0, 0}),
      Suggestion({ 0, 0, 0}),
      Suggestion({ 0, 0, 0}),
      Suggestion({ 0, 0, 0}),
      Suggestion({ 0, 0, 0}),
      Suggestion({ 0, 0, 0}),
      Suggestion({ 0, 0, 0}),
      Suggestion({ 0, 0, 0}),
      Suggestion({ 0, 0, 0}),
      Suggestion({ 0, 0, 0})
    };

};

#endif
