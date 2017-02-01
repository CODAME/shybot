#include <Arduino.h>
#include "helpers.h"
#include "StoreEntry.h"
#include "sensor/GPSSensor.h"

#include <stdio.h>

#define CSV_HEADER "timestamp,heading,latitude,longitude,altitude,proximity_N,proximity_NE,proximity_E,proximity_SE,proximity_S,proximity_SW,proximity_W,proximity_NW"


char csvbuffer[512];

StoreEntry::StoreEntry() {
};

StoreEntry::~StoreEntry() {
  for(int i=0; i<proximityCur; i++) {
    delete proximities[i];
  }
}

int StoreEntry::setHeading(HeadingSensor::Heading heading) {
  this->heading = heading;
  return 0;
};

int StoreEntry::addProximity(ProximitySensor::Proximity *proximity) {
  if(proximityCur == MAX_PROXIMITIES - 1) {
    DEBUG(F("Too many proximities."));
    return 1;
  }
  this->proximities[proximityCur++] = proximity;
  return 0;
};

int StoreEntry::numProximities() {
  return proximityCur;
}

const char* StoreEntry::getCSVHeaders() {
  return CSV_HEADER;
}

const char* StoreEntry::getCSVLocation() {
  String foo = String("foo"); //this makes the ATSAM21D able to convert floats :/

  snprintf(
    csvbuffer,
    256,
    "%lu,%.8f,%.8f",
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
    "%lu,%.2f,%.8f,%.8f,%.2f,%.2f,%.2f,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu",
    sbGetTime(),
    heading.degrees,
    position.lat,
    position.lon,
    position.altitude,
    position.kph,
    rpm.rpm,
    (uint32_t) 0,
    proximities[0]->distance,
    (uint32_t) 0,
    (uint32_t) 0,
    (uint32_t) 0,
    (uint32_t) 0,
    (uint32_t) 0,
    (uint32_t) 0,
    proximities[1]->distance
  );
  return csvbuffer;
}
