#include <Arduino.h>
#include <LinkedList.h>

class StoreEntry {
  struct Position {
    String lat;
    String lon;
    String serialize() {
      return "\"lat\": \"" + String(lat) + "\",\n"
           + "\"lon\": \"" + String(lon) + "\"";
    }
  };

  struct Proximity {
    String direction;
    float distance;
    String serialize() {
      return "\"direction\": \"" + String(direction) + "\",\n"
           + "\"distance\": " + String(distance);
    }
  };

  struct Heading {
    float degrees;
    String serialize() {
      return "\"degrees\": " + String(degrees);
    };
  };

  public:
    Heading heading;
    Position position;
    LinkedList<Proximity> proximities;

    StoreEntry(void(*FUNC_DEBUG) (String));
    ~StoreEntry();

    int setHeading(float heading);

    int setPosition(String lat, String lon);

    int addProximity(String direction, float distance);

    String serialize();

    int log();


};
