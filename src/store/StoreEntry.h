#ifndef SB_STORENTRY
#define SB_STORENTRY
#include <Arduino.h>
#include "../sensor/GPSSensor.h"
#include "../sensor/HeadingSensor.h"
#include "../sensor/ProximitySensor.h"
#include "../sensor/RPMSensor.h"

#define MAX_PROXIMITIES 8

class StoreEntry {
  public:
    StoreEntry();
    ~StoreEntry();

    int setHeading(HeadingSensor::Heading heading);

    int addProximity(ProximitySensor::Proximity *proximity);

    int numProximities();

    const char* getCSVHeaders();

    const char* getHeading();

    const char* getCSVLocation();

    const char* getCSV();

    HeadingSensor::Heading heading;
    GPSSensor::Position position;
    ProximitySensor::Proximity *proximities[MAX_PROXIMITIES];
    RPMSensor::RPM rpm;

  private:
    int proximityCur = 0;



};
#endif
