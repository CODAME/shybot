#include <Adafruit_FONA.h>
#include "helpers.h"
#include "GPSSensor.h"


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
  if(!gpsSuccess && fona->getNetworkStatus() == 1) {
    gsmSuccess = fona->getGSMLoc(&latitude, &longitude);
  }
  return Position({ latitude, longitude, altitude, speedKPH });
};
