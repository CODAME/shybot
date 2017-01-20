#ifndef SB_GPS_SENSOR
#define SB_GPS_SENSOR
#include <Adafruit_FONA.h>

class GPSSensor {
  public:
    struct Position {
      float lat;
      float lon;
      float altitude;
      float kph;
    };
    GPSSensor(Adafruit_FONA *fona);
    Position getPosition();
    bool gpsSuccess = false;
    bool gsmSuccess = false;

  private:
    Adafruit_FONA *fona;
};
#endif
