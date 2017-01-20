#include <Arduino.h>
#include "helpers.h"
#include "StoreEntry.h"
#include "sensor/GPSSensor.h"

StoreEntry::StoreEntry() {
};
StoreEntry::~StoreEntry() {
  delete &heading;
  delete &position;
  delete &proximities;
}

int StoreEntry::setHeading(float heading) {
  this->heading = { heading };
  return 0;
};

int StoreEntry::setPosition(GPSSensor::Position position) {
  this->position = position;
  return 0;
}

int StoreEntry::addProximity(String direction, float distance) {
  if(proximityCur == NUM_PROXIMITIES -1) {
    DEBUG(F("Too many proximities."));
    return 1;
  }
  this->proximities[proximityCur++] = { direction, distance };
  return 0;
};

String StoreEntry::getCSVHeaders() {
  return F("timestamp,heading,latitude,longitude,altitude,proximity_1,proximity_2,proximity_3,proximity_4,proximity_5,proximity_6,proximity_7,proximity_8");
};

String StoreEntry::getCSV() {
    return String(sbGetTime()) + ","
      + heading.degrees + ","
      + position.lat + ","
      + position.lon + ","
      + position.altitude;
}
