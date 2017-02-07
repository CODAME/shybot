#ifndef SB_STORENTRY
#define SB_STORENTRY
#include <Arduino.h>
#include "../sensor/GPSSensor.h"
#include "../sensor/ProximitySensor.h"
#include "../sensor/RPMSensor.h"

#define NUM_PROXIMITY 8

class StoreEntry {
  public:
    StoreEntry();
    ~StoreEntry();

    int addProximity(ProximitySensor::Proximity *proximity);

    int numProximities();

    const char* getCSVHeaders();

    const char* getHeading();

    const char* getCSVLocation();

    const char* getCSV();

    GPSSensor::Position position;
    ProximitySensor::Proximity *proximity[NUM_PROXIMITY];
    RPMSensor::RPM rpm;

};
#endif
