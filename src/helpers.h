#ifndef SB_HELPERS
#define SB_HELPERS

#include <Arduino.h>
#include <Adafruit_FONA.h>

uint32_t sbGetTime();

int sbSetTimeOffset(time_t currentTime);

void DEBUG(String err);
void DEBUG(const char *err);
void DEBUG(float err);
void DEBUG(double err);
void DEBUG(int err);
void DEBUG(uint32_t err);

#endif
