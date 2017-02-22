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

double degToRad(double deg) {
  return (deg - 180) * M_PI / 180;
}

double equiDistance(double *p0, double *p1) {
  int R = 6371 * 1000; // metres
  double lat0 = degToRad(p0[0]);
  double lat1 = degToRad(p1[0]);
  double x = (lat1 - lat0) * cos((lat0 + lat1)/2);
  double y = lat1 - lat0;
  return sqrt(x*x + y*y) * R;
}

uint32_t sbGetTime() {
  return (millis() / 1000) + sbTimeOffsetMS;
}

int sbSetTimeOffset(time_t currentTime) {
  sbTimeOffsetMS = currentTime - millis() / 1000;
  return 0;
}

extern "C" char *sbrk(int i);

int freeRam() {
  char stack_dummy = 0;
  return &stack_dummy - sbrk(0);
}
