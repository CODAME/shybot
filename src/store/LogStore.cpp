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
  double tn = entry->proximity[SENSOR_ORIENTATION_N]->delta;
  uint32_t pne = entry->proximity[SENSOR_ORIENTATION_NE]->distance;
  double tne = entry->proximity[SENSOR_ORIENTATION_NE]->delta;
  uint32_t pnw = entry->proximity[SENSOR_ORIENTATION_NW]->distance;
  double tnw = entry->proximity[SENSOR_ORIENTATION_NW]->delta;
  uint32_t pw = entry->proximity[SENSOR_ORIENTATION_W]->distance;
  double tw = entry->proximity[SENSOR_ORIENTATION_W]->delta;
  uint32_t pe = entry->proximity[SENSOR_ORIENTATION_E]->distance;
  double te = entry->proximity[SENSOR_ORIENTATION_E]->delta;
  uint32_t ps = entry->proximity[SENSOR_ORIENTATION_S]->distance;
  double ts = entry->proximity[SENSOR_ORIENTATION_S]->delta;
  Serial.println(String("        ") + pn + "~" + tn + "      ");
  Serial.println(String("  ") + pnw + "~" + tnw + "------" + pne + "~" + tne + "  ");
  Serial.println(String("") + pw + "~" + tw + "-----------" + pe + "~" + te + "");
  Serial.println(String("        ") + ps + "~" + ts + "      ");
  return 0;
}
