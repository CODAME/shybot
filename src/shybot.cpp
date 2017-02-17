#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_FONA.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_FONA.h>
#include <Adafruit_MCP23017.h>
#include <MCP3008.h>

#include "credentials.h"
#include "constants.h"
#include "helpers.h"

#include "sensor/GPSSensor.h"
#include "sensor/ProximitySensor.h"
#include "sensor/RPMSensor.h"
#include "store/SDStore.h"
#include "store/LogStore.h"
#include "store/IOStore.h"
#include "navigation/navigator.h"


#define PIN_SD_CS 4
#define PIN_ADC_CS A5
#define PIN_SAFE_TO_MOVE A0
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
MCP3008 adc = MCP3008(PIN_ADC_CS);

GPSSensor *gps;
RPMSensor *rpm;
StoreEntry *storeEntry;
LogStore *logStore;
SDStore *sdStore;
IOStore *ioStore;
Navigator *navigator;
ProximitySensor *proximitySensors[NUM_PROXIMITY] = {
  new ProximitySensor(&adc, SENSOR_ORIENTATION_N),
  new ProximitySensor(&adc, SENSOR_ORIENTATION_NE),
  new ProximitySensor(&adc, SENSOR_ORIENTATION_E),
  nullptr,
  new ProximitySensor(&adc, SENSOR_ORIENTATION_S),
  nullptr,
  new ProximitySensor(&adc, SENSOR_ORIENTATION_W),
  new ProximitySensor(&adc, SENSOR_ORIENTATION_NW)
};

void readSensors() {
  #if FONA_ENABLED
    storeEntry->position = gps->getPosition();
  #endif
  rpm->getRPM(&storeEntry->rpm);
  for(int i=0; i<NUM_PROXIMITY; i++) {
    if(proximitySensors[i] == nullptr) { continue; }
    proximitySensors[i]->getProximity(storeEntry->proximity[i]);
  }
}
volatile bool SAFE_TO_MOVE = false;
bool hasCalibrated = false;

void ISR_onSafe() {
  SAFE_TO_MOVE = true;
}

volatile bool DANGER = false;

void ISR_onMotion() {
    DANGER = true;
}

void setup(void)
{
  Serial.begin(9600);
  navigator = new Navigator(PIN_DRIVE, PIN_STEER);
  navigator->calibrate();

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
  pinMode(PIN_MCP_INTERRUPT, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_MCP_INTERRUPT), ISR_onMotion, FALLING);

  pinMode(PIN_SAFE_TO_MOVE, INPUT_PULLUP);
  attachInterrupt(PIN_SAFE_TO_MOVE, ISR_onSafe, FALLING);

  rpm = new RPMSensor(PIN_RPM);

  digitalWrite(PIN_PROXIMITY_TRIGGER, LOW);
  SPI.begin();
  adc.begin();

  sdStore = new SDStore("readings.txt", PIN_SD_CS);
  logStore = new LogStore();
  storeEntry = new StoreEntry();
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
  logStore->graph(storeEntry);
  //if (SAFE_TO_MOVE) {
    navigator->go(storeEntry);
  //}

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
  delay(20);
}
