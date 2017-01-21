#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_FONA.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_FONA.h>

#include "credentials.h"
#include "helpers.h"

#include "sensor/HeadingSensor.h"
#include "sensor/GPSSensor.h"
#include "store/SDStore.h"
#include "store/LogStore.h"
#include "store/IOStore.h"

#define PIN_SD_CS 4
#define PIN_FONA_RST 13

HardwareSerial *fonaSerial = &Serial1;

Adafruit_FONA fona = Adafruit_FONA(PIN_FONA_RST);
Adafruit_MQTT_FONA mqtt(&fona, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

HeadingSensor *heading;
GPSSensor *gps;
StoreEntry *storeEntry;
SDStore *sdStore;
LogStore *logStore;
IOStore *ioStore;


void setup(void)
{
  //while(!Serial) {}
  Serial.begin(9600);

  fonaSerial->begin(4800);
  if(!fona.begin(*fonaSerial)) {
    DEBUG("Failed to communicate with FONA");
    while(true) {}
  }
  fona.enableGPS(true);
  fona.setGPRSNetworkSettings(F(FONA_APN), F(""), F(""));
  heading = new HeadingSensor();
  gps = new GPSSensor(&fona);
  storeEntry = new StoreEntry();
  sdStore = new SDStore("readings.txt", PIN_SD_CS);
  logStore = new LogStore();
  ioStore = new IOStore(&fona, &mqtt);
}

void loop(void)
{
  storeEntry->setHeading(heading->getHeading());
  storeEntry->position = gps->getPosition();
  //sdStore->store(storeEntry);
  int ioStatus = ioStore->store(storeEntry);
  if(ioStatus != IOSTORE_SUCCESS ) {
    DEBUG(F("Failed to upload store."));
  }
  logStore->store(storeEntry);

  delay(2000);
}
