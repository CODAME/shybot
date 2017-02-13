#ifndef SB_PROXIMITY
#define SB_PROXIMITY
#include <Arduino.h>
#include <constants.h>
#include <MCP3008.h>

#define SB_PROX_TRIGGER_INTERVAL_MS 1000
#define SB_PROX_TRIGGER_LEN_US 30


class ProximitySensor {
  public:
    struct Proximity {
      sensor_orientation orientation;
      uint32_t distance;
    };

    ProximitySensor(MCP3008 *adc, sensor_orientation orientation);

    void getProximity(Proximity *proximity);
    int getChannel();
    static void setTriggerPin(int pin);

private:
    void triggerIfNeeded();
    sensor_orientation orientation;
    MCP3008 *_adc;
    int adc_pin[8] = { 7, 6, 5, -1, 4, -1, 2, 3};
};

#endif
