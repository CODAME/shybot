#include <Adafruit_FONA.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_FONA.h>

#include "credentials.h"
#include "helpers.h"

#include "IOStore.h"

#define MAXTXFAILURES 5

#define LOCATION_FEED AIO_USERNAME "/feeds/location"

#define halt(s) { DEBUG(F( s )); delay(1000); NVIC_SystemReset(); }

int txfailures = 0;
bool networkReady = false;
bool mqttReady = false;
const uint8_t QOS_LEVEL=0;

IOStore::IOStore(Adafruit_FONA *myFona, Adafruit_MQTT_FONA *myMqtt) {
  fona = myFona;
  mqtt = myMqtt;
  locationFeed = new Adafruit_MQTT_Publish(mqtt, "mhzmaster/feeds/location/csv", QOS_LEVEL);
};

int IOStore::connectNetwork() {
  if(fona->getNetworkStatus() != 1) {
    return IOSTORE_NET_NOT_READY;
  }

  fona->enableGPRS(false);
  if(!fona->enableGPRS(true)) {
    DEBUG(F("Failed to turn GPRS on."));
    return IOSTORE_NET_NOT_READY;
  }

  networkReady = true;

  return IOSTORE_SUCCESS;
}

int IOStore::ensureConnected() {
  int ready, netStatus;
  ready = fona->TCPconnected() && txfailures < MAXTXFAILURES;
  //if fona has gone offline since initializing, change status
  if(ready) {
    return IOSTORE_SUCCESS;
  }
  DEBUG(F("MQTT not ready. Failures:"));
  DEBUG(txfailures);
  networkReady = false;
  mqttReady = false;
  if(!networkReady) {
    netStatus  = connectNetwork();
  }
  if(netStatus != IOSTORE_SUCCESS) {
    return netStatus;
  } else if(!mqttReady) {
    return connectMQTT();
  } else {
    return IOSTORE_SUCCESS;
  }
}

int IOStore::connectMQTT() {
  DEBUG(F("Connecting to MQTT"));

  int8_t ret, retries = 5;

  while (retries && (ret = mqtt->connect()) != 0) {
    switch (ret) {
      case 1: Serial.println(F("Wrong protocol")); break;
      case 2: Serial.println(F("ID rejected")); break;
      case 3: Serial.println(F("Server unavail")); break;
      case 4: Serial.println(F("Bad user/pass")); break;
      case 5: Serial.println(F("Not authed")); break;
      case 6: Serial.println(F("Failed to subscribe")); break;
      default: Serial.println(F("Connection failed")); break;
    }

    Serial.println(F("Retrying MQTT connection"));
    retries--;
    if (retries == 0) halt("Resetting system");
    delay(5000);
  }

  DEBUG(F("MQTT Connected"));
  txfailures = 0;

  mqttReady = true;
  return IOSTORE_SUCCESS;
}

int IOStore::store(StoreEntry *entry) {
  int fonaStatus = ensureConnected();
  if(fonaStatus != IOSTORE_SUCCESS) {
    return fonaStatus;
  }
  DEBUG(F("We have a connection to IO."));
  if(!locationFeed->publish(entry->getCSVLocation())) {
    return IOSTORE_NET_FAILURE;
  } else {
    return IOSTORE_SUCCESS;
  }
}
