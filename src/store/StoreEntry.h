#include <Arduino.h>
#include <LinkedList.h>

class StoreEntry {
  struct Position {
    String lat;
    String lon;
    String toJSON() {
      return "\"lat\": \"" + String(lat) + "\",\n"
           + "\"lon\": \"" + String(lon) + "\"";
    }
  };

  struct Proximity {
    String direction;
    float distance;
    String toJSON() {
      return "\"direction\": \"" + String(direction) + "\",\n"
           + "\"distance\": " + String(distance);
    }
  };

  struct Heading {
    float degrees;
    String toJSON() {
      return "\"degrees\": " + String(degrees);
    };
  };

  public:
    StoreEntry(void(*FUNC_DEBUG) (String));
    ~StoreEntry();

    int setHeading(float heading);

    int setPosition(String lat, String lon);

    int addProximity(String direction, float distance);

    String toJSON();

    int log();

  private:
    Heading heading;
    Position position;
    LinkedList<Proximity> proximities;

    void(*DEBUG) (String);



};
