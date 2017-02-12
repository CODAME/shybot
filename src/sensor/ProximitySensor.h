#ifndef SB_PROXIMITY
#define SB_PROXIMITY
#include <Arduino.h>
#include <constants.h>

#define SB_PROX_TRIGGER_INTERVAL_MS 1000
#define SB_PROX_TRIGGER_LEN_US 30


class ProximitySensor {
  public:
    struct Proximity {
      sensor_orientation orientation;
      uint32_t distance;
    };

    ProximitySensor(int pin, sensor_orientation orientation);

    void getProximity(Proximity *proximity);
    static void setTriggerPin(int pin);

private:
    void triggerIfNeeded();
    sensor_orientation orientation;
    int pin;
};

#endif
