#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_FONA.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_FONA.h>

#include "credentials.h"
#include "helpers.h"

#include "sensor/HeadingSensor.h"
#include "sensor/GPSSensor.h"
#include "sensor/ProximitySensor.h"
#include "sensor/RPMSensor.h"
#include "store/SDStore.h"
#include "store/LogStore.h"
#include "store/IOStore.h"
#include "navigation/navigator.h"


#define PIN_SONAR_1 A3
#define PIN_SONAR_2 A2
#define PIN_SD_CS 4
#define PIN_STEER 9
#define PIN_DRIVE 10
#define PIN_RPM 11
#define PIN_FONA_RST 13

HardwareSerial *fonaSerial = &Serial1;

Adafruit_FONA fona = Adafruit_FONA(PIN_FONA_RST);
Adafruit_MQTT_FONA mqtt(&fona, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

HeadingSensor *heading;
GPSSensor *gps;
RPMSensor *rpm;
StoreEntry *storeEntry;
LogStore *logStore;
SDStore *sdStore;
IOStore *ioStore;
ProximitySensor proximity1 = ProximitySensor(PIN_SONAR_1, SENSOR_ORIENTATION_NW, SENSOR_TYPE_SONAR);
ProximitySensor proximity2 = ProximitySensor(PIN_SONAR_2, SENSOR_ORIENTATION_NE, SENSOR_TYPE_SONAR);
Navigator *navigator;

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
  rpm = new RPMSensor(PIN_RPM);
  sdStore = new SDStore("readings.txt", PIN_SD_CS);
  logStore = new LogStore();
  ioStore = new IOStore(&fona, &mqtt);
  navigator = new Navigator(PIN_DRIVE, PIN_STEER);
  pinMode(A5, INPUT);
  digitalWrite(A5, HIGH);
}

void loop(void)
{
  delete storeEntry;
  storeEntry = new StoreEntry();
  storeEntry->setHeading(heading->getHeading());
  storeEntry->position = gps->getPosition();
  storeEntry->rpm = rpm->getRPM();
  DEBUG(storeEntry->rpm.rpm);
  storeEntry->addProximity(proximity1.getProximity());
  storeEntry->addProximity(proximity2.getProximity());
  sdStore->store(storeEntry);
  int ioStatus = ioStore->store(storeEntry);
  if(ioStatus != IOSTORE_SUCCESS ) {
    DEBUG(F("Failed to upload store."));
  }
  logStore->store(storeEntry);
  navigator->go(storeEntry);
  delay(500);
}
