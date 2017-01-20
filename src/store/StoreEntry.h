#ifndef SB_STORENTRY
#define SB_STORENTRY
#include <Arduino.h>
#include "../sensor/GPSSensor.h"
#include "../sensor/HeadingSensor.h"

#define NUM_PROXIMITIES 8

class StoreEntry {
  public:
    struct Proximity {
      String direction;
      float distance;
    };

    StoreEntry();
    ~StoreEntry();

    int setHeading(HeadingSensor::Heading heading);

    int setPosition(GPSSensor::Position position);

    int addProximity(String direction, float distance);

    String getCSVHeaders();

    String getCSV();


    HeadingSensor::Heading heading;
    GPSSensor::Position position;
    Proximity proximities[NUM_PROXIMITIES];

  private:
    void(*DEBUG) (String);
    int proximityCur = 0;



};
#endif
