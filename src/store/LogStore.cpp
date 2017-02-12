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
