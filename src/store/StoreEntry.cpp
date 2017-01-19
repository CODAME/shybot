#include <Arduino.h>
#include <ArduinoJson.h>
#include "StoreEntry.h"

StoreEntry::StoreEntry(void(*FUNC_DEBUG) (String)) {
  this->DEBUG = FUNC_DEBUG;
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

int StoreEntry::setPosition(float lat, float lon) {
  this->position = { lat, lon };
  return 0;
}

int StoreEntry::addProximity(String direction, float distance) {
  this->proximities.add({ direction, distance });
  return 0;
};

JsonObject& StoreEntry::toJSON() {
  StaticJsonBuffer<1024> buf;
  JsonObject& root = buf.createObject();
  heading.toJSON(root, "heading");
  position.toJSON(root, "position");
  JsonArray& proximitiesJson = root.createNestedArray("proximities");
  for(int i = 0; i < proximities.size(); i++) {
    JsonObject& node = buf.createObject();
    proximities.get(i).toJSON(node);
    proximitiesJson.add(node);
  }
  return root;
};
