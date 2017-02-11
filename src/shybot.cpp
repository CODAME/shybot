#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_FONA.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_FONA.h>
#include <Adafruit_MCP23017.h>

#include "credentials.h"
#include "constants.h"
#include "helpers.h"

#include "sensor/GPSSensor.h"
#include "sensor/MotionSensor.h"
#include "sensor/ProximitySensor.h"
#include "sensor/RPMSensor.h"
#include "store/SDStore.h"
#include "store/LogStore.h"
#include "store/IOStore.h"
#include "navigation/navigator.h"


#define PIN_SD_CS 4
#define PIN_MCP_INTERRUPT 5
#define PIN_PROXIMITY_TRIGGER 6
#define PIN_STEER 9
#define PIN_DRIVE 10
#define PIN_RPM 11
#define PIN_FONA_RST 13

#define FONA_ENABLED false
#define I2C_ADDRESS_MOTION 0

HardwareSerial *fonaSerial = &Serial1;

Adafruit_FONA fona = Adafruit_FONA(PIN_FONA_RST);
Adafruit_MQTT_FONA mqtt(&fona, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);


Adafruit_MCP23017 mcp;

GPSSensor *gps;
RPMSensor *rpm;
StoreEntry *storeEntry;
LogStore *logStore;
SDStore *sdStore;
IOStore *ioStore;
ProximitySensor proximityN  = ProximitySensor(A1, SENSOR_ORIENTATION_N);
ProximitySensor proximityNE = ProximitySensor(A2, SENSOR_ORIENTATION_NE);
ProximitySensor proximityE  = ProximitySensor(A3, SENSOR_ORIENTATION_E);
ProximitySensor proximityS  = ProximitySensor(A4, SENSOR_ORIENTATION_S);
ProximitySensor proximityW  = ProximitySensor(A5, SENSOR_ORIENTATION_W);
ProximitySensor proximityNW = ProximitySensor(A6, SENSOR_ORIENTATION_NW);
MotionSensor motionN = MotionSensor(&mcp, SENSOR_ORIENTATION_N);
MotionSensor motionE = MotionSensor(&mcp, SENSOR_ORIENTATION_E);
MotionSensor motionS = MotionSensor(&mcp, SENSOR_ORIENTATION_S);
MotionSensor motionW = MotionSensor(&mcp, SENSOR_ORIENTATION_W);
Navigator *navigator;

volatile bool DANGER = false;


void readSensors() {
  #if FONA_ENABLED
  storeEntry->position = gps->getPosition();
  #endif
  storeEntry->rpm = rpm->getRPM();
  storeEntry->proximity[SENSOR_ORIENTATION_N]  = proximityN.getProximity();
  storeEntry->proximity[SENSOR_ORIENTATION_NE] = proximityNE.getProximity();
  storeEntry->proximity[SENSOR_ORIENTATION_E]  = proximityE.getProximity();
  storeEntry->proximity[SENSOR_ORIENTATION_S]  = proximityS.getProximity();
  storeEntry->proximity[SENSOR_ORIENTATION_W]  = proximityW.getProximity();
  storeEntry->proximity[SENSOR_ORIENTATION_NW] = proximityNW.getProximity();
  storeEntry->motion[SENSOR_ORIENTATION_N] = motionN.getMotion();
  storeEntry->motion[SENSOR_ORIENTATION_E] = motionE.getMotion();
  storeEntry->motion[SENSOR_ORIENTATION_S] = motionS.getMotion();
  storeEntry->motion[SENSOR_ORIENTATION_W] = motionW.getMotion();
}

void ISR_onMotion() {
    DANGER = true;
}

void setup(void)
{
  Serial.begin(9600);

  #if FONA_ENABLED
  fonaSerial->begin(4800);
  if(!fona.begin(*fonaSerial)) {
    DEBUG("Failed to communicate with FONA");
  }
  fona.enableGPS(true);
  fona.setGPRSNetworkSettings(F(FONA_APN), F(""), F(""));
  ioStore = new IOStore(&fona, &mqtt);
  gps = new GPSSensor(&fona);
  #endif

  mcp.begin(I2C_ADDRESS_MOTION);
  mcp.setupInterrupts(true, false, LOW);
  pinMode(PIN_MCP_INTERRUPT, INPUT);
  digitalWrite(PIN_MCP_INTERRUPT, HIGH);
  attachInterrupt(digitalPinToInterrupt(PIN_MCP_INTERRUPT), ISR_onMotion, FALLING);

  rpm = new RPMSensor(PIN_RPM);
  digitalWrite(PIN_PROXIMITY_TRIGGER, LOW);

  sdStore = new SDStore("readings.txt", PIN_SD_CS);
  logStore = new LogStore();
  storeEntry = new StoreEntry();
  navigator = new Navigator(PIN_DRIVE, PIN_STEER);
}

void uploadQueued() {
  #if FONA_ENABLED
    StoreEntry *ioEntry;
    ioStore->shiftQueue(ioEntry);
    if (ioEntry != storeEntry) {
      delete ioEntry;
    }
  #endif
}

void loop(void)
{
  if(DANGER) {
    Serial.println("DANGER");
  }
  readSensors();
  sdStore->store(storeEntry);
  logStore->store(storeEntry);
  //navigator->go(storeEntry);
  navigator->setSpeed(10.0, Navigator::DIR_FORWARD, storeEntry->rpm);

  #if FONA_ENABLED
    if (!DANGER && navigator->getPower() == Navigator::STOP) {
      ioStore->store(storeEntry);
      while(!DANGER && ioStore->queueLen() > 0) {
        uploadQueued();
      }
    } else {
      ioStore->pushQueue(storeEntry);
      delay(100);
    }
  #endif
}
