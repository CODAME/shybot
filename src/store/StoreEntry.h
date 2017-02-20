#ifndef SB_STORENTRY
#define SB_STORENTRY
#include <Arduino.h>
#include "sensor/BatterySensor.h"
#include "sensor/GPSSensor.h"
#include "sensor/MotionSensor.h"
#include "sensor/ProximitySensor.h"
#include "sensor/RPMSensor.h"

#define NUM_PROXIMITY 8
#define NUM_MOTION 8

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

    const char* getSensorData();

    const char* getModeName();

    BatterySensor::Battery battery;
    GPSSensor::Position position;
    MotionSensor::Motion *motion[NUM_MOTION] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
    ProximitySensor::Proximity *proximity[NUM_PROXIMITY] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
    RPMSensor::RPM rpm = RPMSensor::RPM({ 0, 0});
    int mode;

};
#endif
