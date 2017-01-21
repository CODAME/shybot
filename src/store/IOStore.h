#include <Adafruit_FONA.h>
#include <Adafruit_MQTT_FONA.h>
#include "StoreEntry.h"


enum iostore_status {
  IOSTORE_SUCCESS,
  IOSTORE_NET_NOT_READY,
  IOSTORE_NET_FAILURE,
  IOSTORE_FATAL_ERROR
};

class IOStore {
  public:
    IOStore(Adafruit_FONA *myfona, Adafruit_MQTT_FONA *myMqtt);

    int store(StoreEntry *store);
    int ensureConnected();

  private:

    int connectNetwork();

    int connectMQTT();
    Adafruit_FONA *fona;
    Adafruit_MQTT *mqtt;
    Adafruit_MQTT_Publish *locationFeed;
};
