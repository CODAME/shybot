#include <Arduino.h>
#include "LogStore.h"

#include "helpers.h"

LogStore::LogStore() {
};

int LogStore::store(StoreEntry *entry) {
  DEBUG(entry->getCSV());
  return 0;
};
