#ifndef SB_PROXIMITY
#define SB_PROXIMITY
#include <NewPing.h>
#include <constants.h>

enum sensor_type {
  SENSOR_TYPE_SONAR,
  SENSOR_TYPE_IR
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
