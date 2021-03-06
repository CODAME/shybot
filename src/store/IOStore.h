#include <Adafruit_FONA.h>
#include <Adafruit_MQTT_FONA.h>
#include "StoreEntry.h"

#define QUEUE_SIZE 30

enum iostore_status {
  IOSTORE_SUCCESS,
  IOSTORE_DELAYED,
  IOSTORE_NET_NOT_READY,
  IOSTORE_NET_FAILURE,
  IOSTORE_FATAL_ERROR,
  IOSTORE_INTERRUPTED
};

class IOStore {
  public:
    IOStore(Adafruit_FONA *myfona, Adafruit_MQTT_FONA *myMqtt);

    iostore_status store(StoreEntry *store);
    iostore_status ensureConnected();
    iostore_status pushQueue(StoreEntry *entry);
    iostore_status shiftQueue(StoreEntry *entry);
    int queueLen();

  private:

    iostore_status connectNetwork();
    iostore_status connectMQTT();
    Adafruit_FONA *fona;
    Adafruit_MQTT *mqtt;
    Adafruit_MQTT_Publish *locationFeed;
    Adafruit_MQTT_Publish *headingFeed;
    Adafruit_MQTT_Publish *batteryVoltsFeed;
    Adafruit_MQTT_Publish *sensorFeed;
    Adafruit_MQTT_Publish *modeFeed;
};
