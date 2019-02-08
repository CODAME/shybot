#include <Adafruit_FONA.h>
#include "helpers.h"
#include "GPSSensor.h"
#include <time.h>

#define GPS_RETRIES 10
#define GPS_INTERVAL 30000

char buf[120];

GPSSensor::GPSSensor(Adafruit_FONA *myFona) {
  this->fona = myFona;
};

GPSSensor::Position GPSSensor::getPosition() {
  float latitude = 0.0;
  float longitude = 0.0;
  float speedKPH = 0.0;
  float heading = 0.0;
  float altitude = 0.0;
  gpsSuccess = fona->getGPS(&latitude, &longitude, &speedKPH, &heading, &altitude);
  if (latitude == 0.0 && longitude == 0.0) {
    gpsSuccess = false;
  }
  if(gpsSuccess && !didSetTime) {
    if(sbSetTimeOffset(getTime()) == 0) {
      didSetTime = true;
    }
  }
  return Position({ latitude, longitude, altitude, speedKPH, heading });
};

//for some reason Network time doesn't work on FONA, but GPS time is accurate ;)
 time_t GPSSensor::getTime() {
  struct tm timeobj = {0};
  fona->getGPS(32, buf, 120);
  char *p = buf; p += 4; //capture just the timestamp from CGNSINF value
  char tmp[10];
  strncpy(tmp, p, 4); p += 4;
  timeobj.tm_year = atoi(tmp) - 1900;
  strncpy(tmp, p, 2); tmp[2] = 0; p += 2;
  timeobj.tm_mon = atoi(tmp) - 1;
  strncpy(tmp, p, 2); tmp[2] = 0; p += 2;
  timeobj.tm_mday = atoi(tmp);
  strncpy(tmp, p, 2); tmp[2] = 0; p += 2;
  timeobj.tm_hour = atoi(tmp);
  strncpy(tmp, p, 2); tmp[2] = 0; p += 2;
  timeobj.tm_min = atoi(tmp);
  strncpy(tmp, p, 2); tmp[2] = 0; p += 2;
  timeobj.tm_sec = atoi(tmp);
  return mktime(&timeobj);
};
