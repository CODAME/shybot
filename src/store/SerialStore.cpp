#include <Arduino.h>
#include "SerialStore.h"

SerialStore::SerialStore(void(*FUNC_DEBUG) (String)) {
  this->DEBUG = FUNC_DEBUG;
};

int SerialStore::store(StoreEntry *entry) {
  char buffer[1024];
  entry->toJSON().printTo(buffer, sizeof(buffer));
  Serial.println(buffer);
  return 0;
};
