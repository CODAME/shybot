#include <Arduino.h>
#include "LogStore.h"
#include "store/StoreEntry.h"

#include "helpers.h"

LogStore::LogStore() {
};

int LogStore::store(StoreEntry *entry) {
  Serial.println(entry->getCSVHeaders());
  Serial.println(entry->getCSV());
  return 0;
};

int LogStore::graph(StoreEntry *entry) {
  uint32_t pne = entry->proximity[SENSOR_ORIENTATION_NE]->distance;
  uint32_t pnw = entry->proximity[SENSOR_ORIENTATION_NW]->distance;
  uint32_t ps = entry->proximity[SENSOR_ORIENTATION_S]->distance;
  const char* mn = entry->motion[SENSOR_ORIENTATION_N]->moving ? "m" : " ";
  const char* me = entry->motion[SENSOR_ORIENTATION_E]->moving ? "m" : " ";
  const char* ms = entry->motion[SENSOR_ORIENTATION_S]->moving ? "m" : " ";
  const char* mw = entry->motion[SENSOR_ORIENTATION_W]->moving ? "m" : "-";
  Serial.println(String("  ") + pnw + "--" + mn + "--" + pne + "  ");
  Serial.println(String("   ") + mw + "---------" + me + "");
  Serial.println(String("      ") + ps + ms + "      ");
  return 0;
}
