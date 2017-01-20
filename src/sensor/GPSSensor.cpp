#include <Adafruit_FONA.h>
#include "GPSSensor.h"


GPSSensor::GPSSensor(Adafruit_FONA *myFona) {
  this->fona = myFona;
};

GPSSensor::Position GPSSensor::getPosition() {
  gpsSuccess = false;
  gsmSuccess = false;
  latitude = 0;
  longitude = 0;
  speedKPH = 0;
  heading = 0;
  altitude = 0;
  gpsSuccess = !fona->getGPS(&latitude, &longitude, &speedKPH, &heading, &altitude);
  if(!gpsSuccess && fona->getNetworkStatus() == 1) {
    gsmSuccess = fona->getGSMLoc(&latitude, &longitude);
  }
  return Position({ latitude, longitude, altitude, speedKPH });
};
