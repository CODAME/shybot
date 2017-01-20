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

  private:
    Adafruit_FONA *fona;
    boolean gpsSuccess, gsmSuccess;
    float latitude, longitude, speedKPH, heading, speedMPH, altitude;
};
#endif
