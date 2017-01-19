#pragma once

#include <Arduino.h>
#include <LinkedList.h>
#include <ArduinoJson.h>

class StoreEntry {
  struct Position {
    float lat;
    float lon;
    int toJSON(JsonObject& node, const char* key) {
      JsonObject& child = node.createNestedObject(key);
      child.set("lat", lat, 10);
      child.set("lon", lon, 10);
      return 0;
    };
  };

  struct Proximity {
    String direction;
    float distance;
    int toJSON(JsonObject& node) {
      node["direction"] = direction;
      node["distance"] = distance;
      return 0;
    }
  };

  struct Heading {
    float degrees;
    int toJSON(JsonObject& node, const char* key) {
      JsonObject& child = node.createNestedObject(key);
      child.set("degrees", degrees);
      return 0;
    };
  };

  public:
    StoreEntry(void(*FUNC_DEBUG) (String));
    ~StoreEntry();

    int setHeading(float heading);

    int setPosition(float lat, float lon);

    int addProximity(String direction, float distance);

    JsonObject& toJSON();

  private:
    Heading heading;
    Position position;
    LinkedList<Proximity> proximities;

    void(*DEBUG) (String);



};
