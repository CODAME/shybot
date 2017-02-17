#ifndef SB_PROXIMITY
#define SB_PROXIMITY
#include <Arduino.h>
#include <constants.h>
#include <MCP3008.h>
#include <LinkedList.h>

#define SB_PROX_TRIGGER_INTERVAL_MS 1000
#define SB_PROX_TRIGGER_LEN_US 30
#define NUM_READINGS 10


class ProximitySensor {
  public:
    struct Proximity {
      sensor_orientation orientation;
      uint32_t distance;
      uint32_t millis;
      double delta;
      double avg;
      double trend;
      double stdDev;
    };

    ProximitySensor(MCP3008 *adc, sensor_orientation orientation);

    void getProximity(Proximity *proximity);
    void clearHistory();
    int getChannel();
    static void setTriggerPin(int pin);

  private:
    void triggerIfNeeded();
    double delta();
    double avg();
    double stdDev();
    double simpleSlope();
    double linRegSlope();
    LinkedList<Proximity*> readings = LinkedList<Proximity*>();
    sensor_orientation orientation;
    MCP3008 *_adc;
    int adc_pin[8] = { 7, 6, 5, -1, 4, -1, 2, 3};
};

#endif
