#include <Arduino.h>
#include <Adafruit_FONA.h>
#include <time.h>


uint32_t sbTimeOffsetMS = 0;
char timebuf[256];

void DEBUG(String err) {
  Serial.println(err);
}

void DEBUG(const char *err) {
  Serial.println(err);
}

void DEBUG(float err) {
  Serial.println(err);
}

void DEBUG(double err) {
  Serial.println(err);
}

void DEBUG(int err) {
  Serial.println(err);
}

void DEBUG(uint32_t err) {
  Serial.println(err);
}

uint32_t sbGetTime() {
  return (millis() / 1000) + sbTimeOffsetMS;
}

int sbSetTimeOffset(time_t currentTime) {
  sbTimeOffsetMS = currentTime - millis() / 1000;
  return 0;
}
