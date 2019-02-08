#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_FONA.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_FONA.h>
#include <Adafruit_MCP23017.h>
#include <Adafruit_SleepyDog.h>
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
#define PIN_FONA_PS A2
#define PIN_PROXIMITY_TRIGGER 6
#define PIN_STEER 9
#define PIN_DRIVE 10
#define PIN_RPM 11
#define PIN_FONA_KEY 12 
#define PIN_FONA_RST 13
#define PIN_BATTERY A0
#define PIN_MOTOR_SWITCH A1
#define PIN_ADC_CS A5

#define MCP_PIN_CALIBRATE 8

#define FONA_ENABLED 1
#define I2C_ADDRESS_MOTION 0

#define RUN_DURATION 1 * 60 * 1000
#define OVERRIDE_DURATION 60 * 1000
#define SLEEP_DURATION 1 * 60 * 1000
#define COMM_MAX_DURATION 5 * 60 * 1000
#define MIN_VOLTS 4.5

bool fonaOn = true; //defaults to ON
uint32_t timeModeChanged = 0;
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
  DEBUG("READING SENSORS");
  rpm->getRPM(&storeEntry->rpm);
  battery->getBattery(&storeEntry->battery);
  for(int i=0; i<NUM_PROXIMITY; i++) {
    if(proximitySensors[i] == nullptr) { continue; }
    proximitySensors[i]->getProximity(storeEntry->proximity[i]);
  }
  if(FONA_ENABLED) {
    storeEntry->position = gps->getPosition();
  }
  DEBUG("READ SENSORS");
}

void toggleFONA(bool turnOn) {
  fonaOn = digitalRead(PIN_FONA_PS);
  if (turnOn != fonaOn) {
    digitalWrite(PIN_FONA_KEY, LOW);
    delay(2000);
    digitalWrite(PIN_FONA_KEY, HIGH);
    delay(3000);
    fonaOn = turnOn;
  }
}

bool startFONA() {
  #if FONA_ENABLED
  toggleFONA(true);
  fonaSerial->begin(4800);
  if(!fona.begin(*fonaSerial)) {
    DEBUG("Failed to communicate with FONA");
    return false;
  }
  Serial.println("setup fona");
  fona.enableGPS(true);
  fona.setGPRSNetworkSettings(F(FONA_APN), F(""), F(""));
  ioStore = new IOStore(&fona, &mqtt);
  gps = new GPSSensor(&fona);
  #endif
  return true;
}

void stopFONA() {
  #if FONA_ENABLED
  toggleFONA(false);
  #endif
}

void setMode(Navigator::nav_mode mode) {
  storeEntry->mode = mode;
  timeModeChanged = millis();
  navigator->startRun(); //just resets the counter, it's safe to run for all changes
}

void setup(void)
{
 // Watchdog.enable(3 * 60 * 1000);
  Serial.begin(9600);
  while(!Serial) {}
  battery = new BatterySensor(PIN_BATTERY);

  pinMode(PIN_FONA_KEY, OUTPUT); 
  digitalWrite(PIN_FONA_KEY, HIGH); 
  pinMode(PIN_FONA_PS, INPUT_PULLUP);
 
  mcp.begin(I2C_ADDRESS_MOTION);

  rpm = new RPMSensor(PIN_RPM);
  //calibrate if button pressed
  mcp.pullUp(MCP_PIN_CALIBRATE, HIGH);
  if(!mcp.digitalRead(MCP_PIN_CALIBRATE)) {
    navigator->calibrate();
  }

  SPI.begin();
  adc.begin();

  //sdStore = new SDStore("readings.txt", PIN_SD_CS);
  logStore = new LogStore();
  storeEntry = new StoreEntry();
  storeEntry->mode = Navigator::RUN;
  navigator = new Navigator(PIN_DRIVE, PIN_STEER, PIN_MOTOR_SWITCH, storeEntry);
  delay(1000);
}

void loop(void)
{
  Watchdog.reset();

  if (storeEntry->mode != Navigator::SLEEP) {
    readSensors();
    //sdStore->store(storeEntry);
    logStore->graph(storeEntry);
  }

  if (storeEntry->mode == Navigator::RUN) {
    DEBUG("Mode: RUN");
    if (storeEntry->battery.volts < MIN_VOLTS) {
      DEBUG("VOLTAGE IS TOO LOW TO RUN");
      setMode(Navigator::SLEEP);
    } else if (millis() - timeModeChanged > RUN_DURATION) {
      navigator->stop();
      setMode(Navigator::SLEEP);
    } else {
      navigator->go();
    }
  } else if (storeEntry->mode == Navigator::OVERRIDE) {
    DEBUG("Mode: OVERRIDE");
    if (millis() - timeModeChanged < OVERRIDE_DURATION) {
      navigator->followOverride();
      navigator->stop();
    } else {
      setMode(Navigator::SLEEP);
    }
  } else if  (storeEntry->mode == Navigator::COMM) {
    bool shouldOverride = false;
    #if FONA_ENABLED
    DEBUG("Mode: COMM");
    startFONA();
    //storeEntry->position = gps->getPosition();
    while(ioStore->ensureConnected() != IOSTORE_SUCCESS && millis() - timeModeChanged < COMM_MAX_DURATION) {
      delay(1000);
      DEBUG("RETRYING IOSTORE");
    }
    ioStore->store(storeEntry);
    stopFONA();
    #else
    DEBUG("FONA disabled. Skipping mode comm.");
    #endif
    //check for override request and set mode accordingly
    if (shouldOverride) {
      setMode(Navigator::OVERRIDE);
    } else {
      setMode(Navigator::RUN);
    }
  } else if (storeEntry->mode == Navigator::SLEEP) {
    DEBUG("Mode: SLEEP");
    if (millis() - timeModeChanged > SLEEP_DURATION) {
      setMode(Navigator::COMM);
    }
  } else {
    DEBUG(String("Mode Unknown: ") + storeEntry->mode);
  }
  DEBUG(String("freeRam: ") + freeRam());

  delay(100);
}
