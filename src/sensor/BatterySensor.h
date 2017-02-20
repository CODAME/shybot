#ifndef SB_BATTERYSENSOR
#define SB_BATTERYSENSOR

#include <Arduino.h>

#define MAX_VOLTS 25

class BatterySensor {
  public:
    struct Battery {
      double volts;
      double percentage() {
        return map(volts, 6.8, 8.2, 0, 100);
      }
    };

    BatterySensor(int pin);

    void getBattery(Battery *battery);

  private:
    int batteryPin;
};
#endif
