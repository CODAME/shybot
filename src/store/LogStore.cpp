#include <Arduino.h>
#include "LogStore.h"

#include "helpers.h"

LogStore::LogStore() {
};

int LogStore::store(StoreEntry *entry) {
  Serial.println(entry->getCSVHeaders());
  Serial.println(entry->getCSV());
  return 0;
};

int LogStore::graph(StoreEntry *entry) {
  uint32_t pn = entry->proximity[SENSOR_ORIENTATION_N]->distance;
  uint32_t pne = entry->proximity[SENSOR_ORIENTATION_NE]->distance;
  uint32_t pnw = entry->proximity[SENSOR_ORIENTATION_NW]->distance;
  uint32_t pw = entry->proximity[SENSOR_ORIENTATION_W]->distance;
  uint32_t pe = entry->proximity[SENSOR_ORIENTATION_E]->distance;
  uint32_t ps = entry->proximity[SENSOR_ORIENTATION_S]->distance;
  const char* mn = entry->motion[SENSOR_ORIENTATION_N]->moving ? "m" : " ";
  const char* me = entry->motion[SENSOR_ORIENTATION_E]->moving ? "m" : " ";
  const char* ms = entry->motion[SENSOR_ORIENTATION_S]->moving ? "m" : " ";
  const char* mw = entry->motion[SENSOR_ORIENTATION_W]->moving ? "m" : "-";
  Serial.println(String("        ") + pn + mn + "      ");
  Serial.println(String("  ") + pnw + "------" + pne + "  ");
  Serial.println(String("") + pw + mw + "-----------" + pe + me + "");
  Serial.println(String("        ") + ps + ms + "      ");
  return 0;
}
