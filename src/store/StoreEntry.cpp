#include <Arduino.h>
#include "StoreEntry.h"

void(*SB_DEBUG) (String);

StoreEntry::StoreEntry(void(*FUNC_DEBUG) (String)) {
  SB_DEBUG = FUNC_DEBUG;
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

int StoreEntry::setPosition(String lat, String lon) {
  this->position = { lat, lon };
  return 0;
}

int StoreEntry::addProximity(String direction, float distance) {
  this->proximities.add({ direction, distance });
  return 0;
};

String StoreEntry::serialize() {
  String ser = "{\n\"heading: {" + heading.serialize() + "},\n"
             + "\"position: {" + position.serialize() + "},\n"
             + "\"proximities: [\n";
  for(int i = 0; i < proximities.size(); i++) {
    ser += "{" + proximities.get(i).serialize() + "},\n";
  }
  ser += "]\n";
  ser += "}";
  return ser;
};

int StoreEntry::log() {
  SB_DEBUG(serialize());
  return 1;
};
