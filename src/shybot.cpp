#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_FONA.h>

#include "helpers.h"

#include "sensor/HeadingSensor.h"
#include "sensor/GPSSensor.h"
#include "store/SDStore.h"
#include "store/LogStore.h"

#define PIN_SD_CS 4
#define PIN_FONA_RST 10

HardwareSerial *fonaSerial = &Serial1;

Adafruit_FONA fona = Adafruit_FONA(PIN_FONA_RST);
HeadingSensor *heading;
GPSSensor *gps;
StoreEntry *storeEntry;
SDStore *sdStore;
LogStore *logStore;


void setup(void)
{
  while(!Serial) {}
  Serial.begin(9600);

  fonaSerial->begin(4800);
  if(!fona.begin(*fonaSerial)) {
    DEBUG("Failed to communicate with FONA");
    while(true) {}
  }
  while(fona.getNetworkStatus() != 1) { delay(1000); }
  fona.enableGPS(true);
  fona.setGPRSNetworkSettings(F("wholesale"), F(""), F(""));
  //fona.enableGPRS(true);
  //heading = new HeadingSensor();
  gps = new GPSSensor(&fona);
  storeEntry = new StoreEntry();
  sdStore = new SDStore("readings.txt", PIN_SD_CS);
  logStore = new LogStore();
}

void loop(void)
{
  DEBUG("Started loop");
  //storeEntry->setHeading(heading->getHeading());
  storeEntry->position = gps->getPosition();
  DEBUG("got position");
  DEBUG(String(0.1));
  //sdStore->store(storeEntry);
  logStore->store(storeEntry);
  DEBUG("stored");

  delay(2000);
}
