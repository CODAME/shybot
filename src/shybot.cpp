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

#include "sensor/BatterySensor.h"
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
#define PIN_BATTERY A0
#define PIN_MOTOR_SWITCH A1
#define PIN_ADC_CS A5

#define FONA_ENABLED 0
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
BatterySensor *battery;
ProximitySensor *proximitySensors[NUM_PROXIMITY] = {
  nullptr,
  new ProximitySensor(&adc, SENSOR_ORIENTATION_NE),
  nullptr,
  nullptr,
  new ProximitySensor(&adc, SENSOR_ORIENTATION_S),
  nullptr,
  nullptr,
  new ProximitySensor(&adc, SENSOR_ORIENTATION_NW),
};
MotionSensor *motionSensors[NUM_MOTION] = {
  new MotionSensor(&mcp, SENSOR_ORIENTATION_N),
  nullptr,
  new MotionSensor(&mcp, SENSOR_ORIENTATION_E),
  nullptr,
  new MotionSensor(&mcp, SENSOR_ORIENTATION_S),
  nullptr,
  new MotionSensor(&mcp, SENSOR_ORIENTATION_W),
  nullptr
};

void readSensors() {
  rpm->getRPM(&storeEntry->rpm);
  if(FONA_ENABLED && storeEntry->rpm.rpm == 0) {
    storeEntry->position = gps->getPosition();
  }
  battery->getBattery(&storeEntry->battery);
  for(int i=0; i<NUM_PROXIMITY; i++) {
    if(proximitySensors[i] == nullptr) { continue; }
    proximitySensors[i]->getProximity(storeEntry->proximity[i]);
  }
  for(int i=0; i<NUM_MOTION; i++) {
    if(motionSensors[i] == nullptr) { continue; }
    motionSensors[i]->getMotion(storeEntry->motion[i]);
  }
}

volatile int buttonPressed = false;
void onButtonPress() {
  buttonPressed = true;
}


void setup(void)
{
  Serial.begin(9600);
  navigator = new Navigator(PIN_DRIVE, PIN_STEER, PIN_MOTOR_SWITCH);
  battery = new BatterySensor(PIN_BATTERY);

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
  mcp.pullUp(8, HIGH);
  mcp.setupInterruptPin(8, FALLING);

  pinMode(PIN_MCP_INTERRUPT, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_MCP_INTERRUPT), onButtonPress, FALLING);

  rpm = new RPMSensor(PIN_RPM);

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
    DEBUG("Uploaded to IO");
  #endif
}
Navigator::Suggestion suggestion;
void loop(void)
{
  readSensors();
  sdStore->store(storeEntry);
  logStore->graph(storeEntry);

  navigator->go(storeEntry);
  if (buttonPressed) {
    DEBUG("INTERRUPT BUTTON PRESSED");
    buttonPressed = false;
  }
  if(!digitalRead(PIN_MCP_INTERRUPT)) {
    DEBUG("INLINE BUTTON PRESSED");
    mcp.digitalRead(8);
  }

  #if FONA_ENABLED
    if (storeEntry->mode == Navigator::STOP) {
      ioStore->store(storeEntry);
      if(ioStore->queueLen() > 0) {
        uploadQueued();
      }
    } else {
      ioStore->pushQueue(storeEntry);
    }
  #endif
  delay(20);
}
