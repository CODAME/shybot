#include <Adafruit_FONA.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_FONA.h>

#include "credentials.h"
#include "helpers.h"

#include "IOStore.h"
#include "navigation/navigator.h"

#define MAXTXFAILURES 5

#define AIO_HTTP_HOST "http://io.adafruit.com/api/v2/"
#define AIO_LAST "/data/retain"

#define LOCATION_FEED AIO_USERNAME "/feeds/location/csv"
#define SENSOR_FEED AIO_USERNAME "/feeds/sensors"
#define DIRECT_DRIVE_FEED AIO_USERNAME "/feeds/direct-drive"
#define DIRECT_DRIVE_FEED_URL AIO_HTTP_HOST DIRECT_DRIVE_FEED AIO_LAST
#define FORCE_DIRECTION_FEED AIO_USERNAME "/feeds/force-direction"
#define FORCE_DIRECTION_FEED_URL AIO_HTTP_HOST FORCE_DIRECTION_FEED AIO_LAST

#define halt(s) { DEBUG(F( s )); delay(1000); NVIC_SystemReset(); }

int txfailures = 0;
bool networkReady = false;
bool mqttReady = false;
const uint8_t QOS_LEVEL=0;

IOStore::IOStore(Adafruit_FONA *myFona, Adafruit_MQTT_FONA *myMqtt) {
  fona = myFona;
  mqtt = myMqtt;
  locationFeed = new Adafruit_MQTT_Publish(mqtt, LOCATION_FEED, QOS_LEVEL);
  sensorFeed = new Adafruit_MQTT_Publish(mqtt, SENSOR_FEED, QOS_LEVEL);
};

iostore_status IOStore::getLast(char* url) {
  uint16_t statusCode;
  int16_t datalen;
  fona->flush();
  if(!fona->HTTP_GET_start(url, &statusCode, (uint16_t *)&datalen)) {
    return IOSTORE_NET_FAILURE;
  }
  int16_t pos = 0;
  while(datalen > 0) {
    while(fona->available()) {
      char c = fona->read();
      httpData[pos++] = c;
      datalen--;
      if(!datalen) break;
    }
  }
  httpData[pos] = 0x00;
  fona->HTTP_GET_end();
  return IOSTORE_SUCCESS;
}

iostore_status IOStore::connectNetwork() {
  uint8_t status = fona->getNetworkStatus()
  if( status != 1 && status != 5) {
    DEBUG(F("Network not ready."));
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

//  mqtt->subscribe(directDriveFeed);

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

iostore_status IOStore::getOverrideValue(char* feedUrl, char* value) {
  iostore_status status = ensureConnected();
  if(status != IOSTORE_SUCCESS) return status;
  DEBUG("CHECKING OVERRIDES");
  status = getLast(feedUrl);
  int16_t pos = 0;
  while(httpData[pos] != 0x00 && httpData[pos] != ',') {
    value[pos] = httpData[pos];
    pos++;
  }
  value[pos] = 0x00;
  Serial.println(value);
  return status;
}

iostore_status IOStore::getOverrides(StoreEntry *entry) {
  iostore_status status = IOSTORE_SUCCESS;
  char directDriveVal[100];
  status = status ? status : getOverrideValue(DIRECT_DRIVE_FEED_URL, directDriveVal);
  char forceDirVal[100];
  status = status ? status : getOverrideValue(FORCE_DIRECTION_FEED_URL, forceDirVal);
  if (strcmp(directDriveVal, "ON") == 0) {
    entry->mode = Navigator::DIRECT;
  } else if (atoi(forceDirVal) != 0) {
    entry->mode = Navigator::FORCE;
    entry->forceDir = atoi(forceDirVal);
  } else {
    DEBUG("NO OVERRIDES FOUND");
  }
  return status;
}

iostore_status IOStore::store(StoreEntry *entry) {
  iostore_status fonaStatus = ensureConnected();
  if(fonaStatus != IOSTORE_SUCCESS) {
    return fonaStatus;
  }
  bool ok = true;
  ok = ok && sensorFeed->publish(entry->getCSV());
  ok = ok && locationFeed->publish(entry->getCSVLocation());
  if (ok) {
    DEBUG("PUBLISHED!");
    return IOSTORE_SUCCESS;
  } else {
    return IOSTORE_NET_FAILURE;
  }
}
