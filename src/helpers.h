#ifndef SB_HELPERS
#define SB_HELPERS

#include <Arduino.h>

uint32_t sbGetTime();

int sbSetTimeOffset(uint32_t current);

void DEBUG(String err);

void DEBUG(const char *err);

#endif
