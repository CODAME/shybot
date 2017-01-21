#ifndef SB_GPS_SENSOR
#define SB_GPS_SENSOR
#include <Adafruit_FONA.h>

class GPSSensor {
  public:
    struct Position { double lat, lon, altitude, kph; };

    GPSSensor(Adafruit_FONA *fona);
    Position getPosition();
    time_t getTime();
    bool gpsSuccess = false;
    bool gsmSuccess = false;
    bool didSetTime = false;

  private:
    Adafruit_FONA *fona;
};
#endif
