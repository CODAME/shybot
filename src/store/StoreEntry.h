#ifndef SB_STORENTRY
#define SB_STORENTRY
#include <Arduino.h>
#include "../sensor/GPSSensor.h"
#include "../sensor/HeadingSensor.h"

#define NUM_PROXIMITIES 8

class StoreEntry {
  public:
    struct Proximity {
      const char *direction;
      float distance;
    };

    StoreEntry();
    ~StoreEntry();

    int setHeading(HeadingSensor::Heading heading);

    int addProximity(const char* direction, float distance);

    const char* getCSVHeaders();

    const char* getCSVLocation();

    const char* getCSV();


    HeadingSensor::Heading heading;
    GPSSensor::Position position;
    Proximity proximities[NUM_PROXIMITIES];

  private:
    int proximityCur = 0;



};
#endif
