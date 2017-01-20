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

  fonaSerial->begin(4800, SERIAL_8N1);
  if(!fona.begin(*fonaSerial)) {
    DEBUG(F("Failed to communicate with FONA"));
    while(true) {}
  }
  char imei[15] = {0};
  uint8_t imeiLen = fona.getIMEI(imei);
  if(imeiLen > 0) {
    DEBUG("IMEI:");
    DEBUG(imei);
  }
  DEBUG(String(fona.getNetworkStatus()));
  delay(2000);
  fona.enableGPS(true);
  fona.setGPRSNetworkSettings(F("wholesale"), F(""), F(""));
  fona.enableGPRS(true);
  heading = new HeadingSensor();
  storeEntry = new StoreEntry();
  sdStore = new SDStore(F("readings.txt"), PIN_SD_CS);
  logStore = new LogStore();
}

void loop(void)
{
  storeEntry->setHeading(heading->getHeadingDegrees());
  storeEntry->setPosition(gps->getPosition());
  //sdStore->store(storeEntry);
  logStore->store(storeEntry);

  delay(2000);
}
