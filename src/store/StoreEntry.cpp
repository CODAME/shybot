#include <Arduino.h>
#include "helpers.h"
#include "StoreEntry.h"
#include "sensor/GPSSensor.h"

#include <stdio.h>

char csvbuffer[512];

StoreEntry::StoreEntry() {
};

int StoreEntry::setHeading(HeadingSensor::Heading heading) {
  this->heading = heading;
  return 0;
};

int StoreEntry::addProximity(const char *direction, float distance) {
  if(proximityCur == NUM_PROXIMITIES -1) {
    DEBUG("Too many proximities.");
    return 1;
  }
  this->proximities[proximityCur++] = { direction, distance };
  return 0;
};

const char* StoreEntry::getCSVHeaders() {
  return "timestamp,heading,latitude,longitude,altitude,proximity_1,proximity_2,proximity_3,proximity_4,proximity_5,proximity_6,proximity_7,proximity_8";
};

const char* StoreEntry::getCSVLocation() {
  String foo = String("foo"); //this makes the ATSAM21D able to convert floats :/

  snprintf(
    csvbuffer,
    256,
    "%lu,%.10f,%.10f",
    sbGetTime(),
    position.lat,
    position.lon
  );
  return csvbuffer;
}

const char* StoreEntry::getCSV() {
  String foo = String("foo"); //this makes the ATSAM21D able to convert floats :/

  snprintf(
    csvbuffer,
    512,
    "%lu,%.2f,%.10f,%.10f,%.2f,%.2f",
    sbGetTime(),
    heading.degrees,
    position.lat,
    position.lon,
    position.altitude,
    position.kph
  );
  return csvbuffer;
}
