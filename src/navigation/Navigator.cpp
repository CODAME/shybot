#include <Arduino.h>
#include <Servo.h>
#include "helpers.h"
#include "constants.h"
#include "navigation/Navigator.h"
#include "store/StoreEntry.h"

#define STEER_MAX 180
#define STEER_MIN 0
#define SPEED_TOLERANCE_KPH .5
#define POWER_INCREMENT 1
#define POWER_INCREMENT_FREQ 500
#define DANGER_FREQ 500
#define TURN_LENGTH 50
#define BACKUP_LENGTH 50
#define BACKUP_TRIES 5
#define RUN_LENGTH 500
#define RUN_MAX_TIME 120000
#define STOP_MS 30000
#define START_POWER 15
#define MAX_THROTTLE 140
#define MAX_THROTTLE_AUTO 120
#define MIN_THROTTLE 20
#define MIN_THROTTLE_AUTO 40

#define DRIVE_TEST 0
#define FONA_TEST 0

Navigator::Navigator(int _drivePin, int _steerPin, int motorSwitchPin, StoreEntry *storeEntry) {
  drivePin = _drivePin;
  steerPin = _steerPin;
  motorSwitch = motorSwitchPin;
  pinMode(motorSwitch, OUTPUT);
  currentEntry = storeEntry;
}

void Navigator::go() {
  drive.attach(drivePin);
  steer.attach(steerPin);
  DEBUG(String("POWER: ") + currentPower);

  DEBUG("RUNNING");
  DEBUG(currentEntry->rpm.rotations - runStart);
  if(currentEntry->rpm.rotations - runStart > RUN_LENGTH ||
    millis() - runStartTime > RUN_MAX_TIME
  ) {
    DEBUG("Already went too far.");
    stop();
    return;
  } else if(currentEntry->rpm.rotations - turnStart < TURN_LENGTH) {
    safelyFollowHeading((180 + initMotionHeading) % 360, 5);
  } else {
    safelyFollowHeading(0, 5);
  }
}

void Navigator::stop() {
  setPower(0, DIR_STOP);
  setSteer(CENTER);
  delay(1000);
  steer.detach();
  drive.detach();
}

void Navigator::followOverride() {
  //do override behavior
}

void Navigator::startRun() {
  runStart = currentEntry->rpm.rotations;
  runStartTime = millis();
}

void Navigator::startBackup(int heading) {
  DEBUG("START BACKUP");
  backupHeading = heading;
  backupGoal = currentEntry->rpm.rotations + BACKUP_LENGTH;
  //if(millis() % 30000 < 5000) {
    countBackups = 0;
  //}
  //countBackups++;
};

void Navigator::stopBackup() {
  backupGoal = currentEntry->rpm.rotations;
}

void Navigator::backup() {
  backup(backupHeading);
}

void Navigator::backup(int heading) {
  DEBUG(String("COUNT BACKUPS: ") + countBackups);
  if (getDanger(DIR_REVERSE)) {
    DEBUG("REVERSE DANGER");
    stopBackup();
  }
  if (backupHeading == 180) {
    setSteer(CENTER);
  } if (backupHeading > 180) {
    setSteer(LEFT);
  } else {
    setSteer(RIGHT);
  }
  setSpeed(7, DIR_REVERSE);
}

int Navigator::getBackupHeading() {
  sensor_orientation orientation = getMinProximity()->orientation;
  if (orientation == SENSOR_ORIENTATION_NE) {
    return 225;
  } else if (orientation == SENSOR_ORIENTATION_NW) {
    return 135;
  } else {
    return 180;
  }
}

void Navigator::safelyFollowHeading(int heading, int speed) {
  if(backupGoal - currentEntry->rpm.rotations > 0) {
    DEBUG(String("backup: ") + (backupGoal - currentEntry->rpm.rotations));
    backup(heading);
  } else if(getDanger()) {
    DEBUG("DANGER");
    startBackup(getBackupHeading());
    setSpeed(0, DIR_STOP);
  } else if( currentPower == 100 && currentEntry->rpm.rotations == runStart) {
    DEBUG("STUCK");
    startBackup(180);
    setSpeed(0, DIR_STOP);
  } else {
    DEBUG(String("HEADING: ") + heading);
    followHeading(heading, speed);
  }
}

void Navigator::followHeading(int heading, int speed) {
  if(heading > 90 && heading < 225) {
    backup();
  } else if ( heading < 10 || heading > 350) {
    setSteer(CENTER);
    setSpeed(speed, DIR_FORWARD);
  } else if (heading < 180) {
    setSteer(LEFT);
    setSpeed(speed, DIR_FORWARD);
  } else {
    setSteer(RIGHT);
    setSpeed(speed, DIR_FORWARD);
  }
}

ProximitySensor::Proximity* Navigator::getMaxProximity() {
  int proxNum = -1;
  double maxProximity = 0;
  for (int i=0; i<NUM_PROXIMITY; i++) {
    if(currentEntry->proximity[i]->distance > maxProximity) {
      maxProximity = currentEntry->proximity[i]->distance;
      proxNum = i;
    }
  }
  return currentEntry->proximity[proxNum];
}

ProximitySensor::Proximity* Navigator::getMinProximity() {
  int proxNum = -1;
  double minProximity = 5000;
  for (int i=0; i<NUM_PROXIMITY; i++) {
    double distance = currentEntry->proximity[i]->distance;
    if(distance != 0 && distance < minProximity) {
      minProximity = distance;
      proxNum = i;
    }
  }
  return currentEntry->proximity[proxNum];
}

bool Navigator::headingIsDanger(int heading, direction direction) {
  if(currentDirection == DIR_FORWARD) {
    DEBUG(String(heading) + "deg is danger");
    return heading < 90 || heading > 270;
  } else {
    return heading > 90 && heading < 270;
  }
}


bool Navigator::getDanger(direction direction) {
  int proxNum = -1;
  double minProximity = 450;
  for (int i=0; i<NUM_PROXIMITY; i++) {
    double distance = currentEntry->proximity[i]->distance;
    double heading = sensor_heading[currentEntry->proximity[i]->orientation];
    if(distance != 0 && distance < minProximity && headingIsDanger(heading, direction)) {
      minProximity = distance;
      proxNum = i;
    }
  }
  if (proxNum != -1 && millis() - lastDanger > DANGER_FREQ) {
    lastDanger = millis();
    return true;
  } else {
    return false;
  }
}

bool Navigator::getDanger() {
  return getDanger(currentDirection);
}


void Navigator::setSpeed(double goalKPH, direction direction) {
  DEBUG(String("POWER: ") + currentPower);
  if (goalKPH == 0) {
    return setPower(0, DIR_STOP);
  }
  if (direction != currentDirection) {
    currentPower = START_POWER;
    drive.write(90);
    delay(1000);
  }
  if (millis() - lastIncrement > POWER_INCREMENT_FREQ ) {
    lastIncrement = millis();
    double diff = goalKPH - currentEntry->rpm.kph();
    double inc = constrain((diff / goalKPH) * POWER_INCREMENT, -POWER_INCREMENT, POWER_INCREMENT);
    if (abs(diff) > SPEED_TOLERANCE_KPH) {
      setPower(constrain(currentPower + inc, 0, 100), direction);
    }
  }
}

void Navigator::setPower(double power, direction direction) {
  if (direction == DIR_STOP) {
    drive.write(90);
  } else if (direction == DIR_FORWARD) {
    drive.write(map(power, 0, 100, 100, MAX_THROTTLE_AUTO));
  } else {
    int mappedPower = map(power, 0, 100, 90, MIN_THROTTLE_AUTO);
    drive.write(mappedPower);
  }
  currentDirection = direction;
  currentPower = power;
}

double Navigator::getPower() {
  return currentPower;
}

void Navigator::setSteer(turn turn) {
  currentTurn = turn;
  steer.write(turn);
}

Navigator::turn Navigator::getSteer() {
  return currentTurn;
}

void Navigator::calibrate() {
    drive.write(MAX_THROTTLE);
    delay(2000);
    drive.write(MIN_THROTTLE);
    delay(2000);
    drive.write(90);
}
