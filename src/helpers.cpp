#include <Arduino.h>

uint32_t sbTimeOffsetMS = 0;

uint32_t sbGetTime() {
  return millis() + sbTimeOffsetMS;
}

int sbSetTimeOffset(uint32_t current) {
  sbTimeOffsetMS = current;
  return 0;
}

void DEBUG(String err) {
  Serial.println(err);
}

void DEBUG(const char *err) {
  Serial.println(err);
}
