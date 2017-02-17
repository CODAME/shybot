#include <Arduino.h>
#include <Servo.h>
#include "helpers.h"
#include "constants.h"
#include "navigation/Navigator.h"

#define STEER_MAX 180
#define STEER_MIN 0
#define SPEED_TOLERANCE_KPH .5
#define POWER_INCREMENT 1
#define MOTION_LED 8
#define BACKUP_TIME 300
#define DANGER_FREQ 500

Navigator::Navigator(int drivePin, int steerPin) {
  drive.attach(drivePin);
  steer.attach(steerPin);
  pinMode(MOTION_LED, OUTPUT);
}

void Navigator::go(StoreEntry *entry) {
  DEBUG(millis() - timer);
  currentEntry = entry;
  int heading = getApproachHeading();

  if (millis() - timer > 20000) {
    timer = millis();
    mode++;
  }
  switch(mode) {
    /*
    case SEARCH:
      DEBUG("SEARCHING");
      search();
      timer++;
      break;
      */
    case STOP:
      DEBUG("STOPPING");
      setSpeed(0, DIR_FORWARD);
      if(millis() - timer > 5000) {
        timer = millis();
        mode++;
      }
      break;
    case SCAN:
      DEBUG("SCANNING");
      if(heading != -1) {
        mode = RUN;
        initMotionHeading = heading;
      } else {
        timer = millis();
      }
      break;
    case RUN:
      DEBUG("RUNNING");
      if(millis() - timer < 5000) {
        safelyFollowHeading((180 + initMotionHeading) % 360, 80);
      } else {
        safelyFollowHeading(0, 80);

      }
      timer++;
      break;
    default:
      mode = STOP;
  };
}

  void Navigator::backup(double heading) {
  if (currentEntry->proximity[SENSOR_ORIENTATION_E]->distance < 350 &&
      currentEntry->proximity[SENSOR_ORIENTATION_W]->distance < 350) {
    setSteer(CENTER);
  } else if (currentEntry->proximity[SENSOR_ORIENTATION_E]->distance < 350) {
    setSteer(LEFT);
  } else if (currentEntry->proximity[SENSOR_ORIENTATION_W]->distance < 350) {
    setSteer(RIGHT);
  } else {
    if (heading > 180) {
      setSteer(LEFT);
    } else {
      setSteer(RIGHT);
    }
  }
  setSpeed(7, DIR_REVERSE);
}

void Navigator::search() {
  ProximitySensor::Proximity *goal = getMaxProximity();
  safelyFollowHeading(goal->orientation);
}

void Navigator::safelyFollowHeading(int heading, int speed) {
  ProximitySensor::Proximity *obstacle = getMinProximity();
  DEBUG(String("HEADING: ") + heading);
  DEBUG(obstacle->avg);
  if(getDanger()) {
    DEBUG("DANGER");
    setSpeed(0, DIR_FORWARD);
  } else if(backupStart && millis() - backupStart < BACKUP_TIME) {
    backup();
  } else if(obstacle->avg < 500) {
    followHeading((sensor_heading[obstacle->orientation] + 180) % 360, speed);
  } else {
    followHeading(heading);
  }
}

void Navigator::followHeading(int heading, int speed) {
  if(heading > 90 && heading < 225) {
    backup(((int) heading + 180) % 360 );
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

int Navigator::getMotionHeading() {
  digitalWrite(MOTION_LED, LOW);
  double sumCos = 0;
  double sumSin = 0;
  int countMotion = 0;
  for(int i=0; i<NUM_MOTION; i++) {
    if(currentEntry->motion[i]->moving) {
      countMotion++;
      sumCos += cos(M_PI * sensor_heading[i] / 180);
      sumSin += sin(M_PI * sensor_heading[i] / 180);
    }
  }
  if(countMotion) {
    digitalWrite(MOTION_LED, HIGH);
    return (int) ((180 * atan2(sumSin/countMotion, sumCos/countMotion) / M_PI) + 360) % 360;
  } else {
    DEBUG("NO MOTION");
    return -1;
  }
}

double Navigator::getAvgProximity() {
  int proxCount = 0;
  double sumProximity = 0;
  for (int i=0; i<NUM_PROXIMITY; i++) {
    if(currentEntry->proximity[i]->distance != 0) {
      sumProximity += currentEntry->proximity[i]->distance;
      proxCount++;
    }
  }
  return sumProximity / proxCount;
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

int Navigator::getApproachHeading() {
  int proxNum = -1;
  double minDelta = -100;
  for (int i=0; i<NUM_PROXIMITY; i++) {
    double avg = currentEntry->proximity[i]->avg;
    double delta = currentEntry->proximity[i]->delta;
    if(avg < 1400 && delta < minDelta) {
      minDelta = delta;
      proxNum = i;
    }
  }
  if (proxNum == -1) {
    return -1;
  } else {
    DEBUG(String("APPROACHING: ") + currentEntry->proximity[proxNum]->delta);
    return sensor_heading[currentEntry->proximity[proxNum]->orientation];
  }

}

ProximitySensor::Proximity* Navigator::getMinProximity() {
  int proxNum = -1;
  double minProximity = 5000;
  for (int i=0; i<NUM_PROXIMITY; i++) {
    double distance = currentEntry->proximity[i]->avg;
    if(distance != 0 && distance < minProximity) {
      minProximity = distance;
      proxNum = i;
    }
  }
  return currentEntry->proximity[proxNum];
}

bool Navigator::getDanger() {
  int proxNum = -1;
  double minProximity = 450;
  for (int i=0; i<NUM_PROXIMITY; i++) {
    double distance = currentEntry->proximity[i]->distance;
    if(distance != 0 && distance < minProximity) {
      minProximity = distance;
      proxNum = i;
    }
  }
  if (proxNum != -1 && millis() - lastDanger < DANGER_FREQ) {
    lastDanger = millis();
    return true;
  } else {
    return false;
  }
}


void Navigator::setSpeed(double goalKPH, direction direction) {
  if (goalKPH == 0) {
    currentPower = 0;
    drive.write(90);
    return;
  }
  if (direction != currentDirection) {
    DEBUG("SWITCHING");
    currentPower = 40;
    drive.write(90);
    delay(1000);
  }
  double diff = goalKPH - currentEntry->rpm.kph();
  double inc = constrain((diff / goalKPH) * POWER_INCREMENT, -POWER_INCREMENT, POWER_INCREMENT);
  if (abs(diff) > SPEED_TOLERANCE_KPH) {
    setPower(constrain(currentPower + inc, 0, 100), direction);
  }
}

void Navigator::setPower(double power, direction direction) {
  if (direction == DIR_FORWARD) {
    drive.write(map(power, 0, 100, 90, MAX_THROTTLE));
  } else {
    int mappedPower = map(power, 0, 100, 90, MIN_THROTTLE);
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

void Navigator::calibrate() {
    drive.write(MAX_THROTTLE);
    delay(2000);
    drive.write(MIN_THROTTLE);
    delay(2000);
    drive.write(90);
}

Navigator::turn Navigator::getSteer() {
  return currentTurn;
}
