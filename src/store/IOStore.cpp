#include <Adafruit_FONA.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_FONA.h>

#include "credentials.h"
#include "helpers.h"

#include "IOStore.h"

#define MAXTXFAILURES 5

#define LOCATION_FEED AIO_USERNAME "/feeds/location/csv"
#define HEADING_FEED AIO_USERNAME "/feeds/heading"
#define SPEED_FEED AIO_USERNAME "/feeds/speed"
#define PROXIMITY_NE_FEED AIO_USERNAME "/feeds/proximity-ne"
#define PROXIMITY_NW_FEED AIO_USERNAME "/feeds/proximity-nw"

#define halt(s) { DEBUG(F( s )); delay(1000); NVIC_SystemReset(); }

int txfailures = 0;
bool networkReady = false;
bool mqttReady = false;
const uint8_t QOS_LEVEL=0;

IOStore::IOStore(Adafruit_FONA *myFona, Adafruit_MQTT_FONA *myMqtt) {
  fona = myFona;
  mqtt = myMqtt;
  locationFeed = new Adafruit_MQTT_Publish(mqtt, LOCATION_FEED, QOS_LEVEL);
  headingFeed = new Adafruit_MQTT_Publish(mqtt, HEADING_FEED, QOS_LEVEL);
  speedFeed = new Adafruit_MQTT_Publish(mqtt, SPEED_FEED, QOS_LEVEL);
  proximityNEFeed = new Adafruit_MQTT_Publish(mqtt, PROXIMITY_NE_FEED, QOS_LEVEL);
  proximityNWFeed = new Adafruit_MQTT_Publish(mqtt, PROXIMITY_NW_FEED, QOS_LEVEL);
};

iostore_status IOStore::connectNetwork() {
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

iostore_status IOStore::ensureConnected() {
  int ready;
  iostore_status netStatus;
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

iostore_status IOStore::connectMQTT() {
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

iostore_status IOStore::store(StoreEntry *entry) {
  iostore_status fonaStatus = ensureConnected();
  if(fonaStatus != IOSTORE_SUCCESS) {
    return fonaStatus;
  }
  if  (locationFeed->publish(entry->getCSVLocation())
    && headingFeed->publish(entry->position.heading)
    && speedFeed->publish(entry->position.kph)
    && proximityNWFeed->publish(entry->proximity[0]->distance)
    && proximityNEFeed->publish(entry->proximity[1]->distance)
      ) {
    return IOSTORE_SUCCESS;
  } else {
    return IOSTORE_NET_FAILURE;
  }
}

iostore_status IOStore::shiftQueue(StoreEntry *entry) {
  //calling function has responsibility to delete *entry after user.
  entry = queue.shift();
  iostore_status ioStatus = store(entry);
  if(ioStatus != IOSTORE_SUCCESS ) {
    DEBUG(F("Failed to upload store."));
  }
  return ioStatus;
}

iostore_status IOStore::pushQueue(StoreEntry *entry) {
  if(queue.size() > QUEUE_SIZE) {
    delete queue.shift();
    queue.add(entry);
  }
  return IOSTORE_SUCCESS;
}

int IOStore::queueLen() {
  return queue.size();
}
