#include <NewPing.h>
#ifndef SB_PROXIMITY
#define SB_PROXIMITY

enum sensor_type {
  SENSOR_TYPE_SONAR,
  SENSOR_TYPE_IR
};

enum sensor_orientation {
  SENSOR_ORIENTATION_N,
  SENSOR_ORIENTATION_NW,
  SENSOR_ORIENTATION_W,
  SENSOR_ORIENTATION_SW,
  SENSOR_ORIENTATION_S,
  SENSOR_ORIENTATION_SE,
  SENSOR_ORIENTATION_E,
  SENSOR_ORIENTATION_NE
};

class ProximitySensor {
  public:
    struct Proximity {
      int type;
      int orientation;
      uint32_t distance;
    };

    ProximitySensor(int pin, sensor_orientation orientation, sensor_type type);

    Proximity* getProximity();

  private:
    NewPing *sonar;
    sensor_orientation orientation;
    sensor_type type;

};

#endif
