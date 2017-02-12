#include <Arduino.h>
#include "helpers.h"
#include "StoreEntry.h"
#include "sensor/GPSSensor.h"

#include <stdio.h>

#define CSV_HEADER "timestamp,heading,latitude,longitude,altitude,kph,rpm,proximity_N,proximity_NE,proximity_E,proximity_S,proximity_W,proximity_NW,motion_N,motion_E,motion_S,motion_W"


char csvbuffer[512];

StoreEntry::StoreEntry() {
  for(int i=0; i<NUM_PROXIMITY; i++) {
    proximity[i] = new ProximitySensor::Proximity({ (sensor_orientation) i, 0 });
  }
  for(int i=0; i<NUM_MOTION; i++) {
    motion[i] = new MotionSensor::Motion({ (sensor_orientation) i, 0 });
  }
};

StoreEntry::~StoreEntry() {
  for(int i=0; i<NUM_PROXIMITY; i++) {
    delete proximity[i];
  }
  for(int i=0; i<NUM_MOTION; i++) {
    delete motion[i];
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
    "%lu,%.2f,%.8f,%.8f,%.2f,%.2f,%.2f,%lu,%lu,%lu,%lu,%lu,%lu,%d,%d,%d,%d",
    sbGetTime(),
    position.heading,
    position.lat,
    position.lon,
    position.altitude,
    position.kph,
    rpm.rpm,
    proximity[SENSOR_ORIENTATION_N]->distance,
    proximity[SENSOR_ORIENTATION_NE]->distance,
    proximity[SENSOR_ORIENTATION_E]->distance,
    proximity[SENSOR_ORIENTATION_S]->distance,
    proximity[SENSOR_ORIENTATION_W]->distance,
    proximity[SENSOR_ORIENTATION_NW]->distance,
    motion[SENSOR_ORIENTATION_N]->moving,
    motion[SENSOR_ORIENTATION_E]->moving,
    motion[SENSOR_ORIENTATION_S]->moving,
    motion[SENSOR_ORIENTATION_W]->moving
  );
  return csvbuffer;
}
