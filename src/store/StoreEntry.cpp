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
  for(int i=0; i<NUM_PROXIMITY; i++) {
    delete proximity[i];
  }
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
    position.heading,
    position.lat,
    position.lon,
    position.altitude,
    position.kph,
    rpm.rpm,
    (uint32_t) 0,
    proximity[SENSOR_ORIENTATION_NW]->distance,
    (uint32_t) 0,
    (uint32_t) 0,
    (uint32_t) 0,
    (uint32_t) 0,
    (uint32_t) 0,
    (uint32_t) 0,
    proximity[SENSOR_ORIENTATION_SE]->distance
  );
  return csvbuffer;
}
