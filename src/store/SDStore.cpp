#include <Arduino.h>
#include "SDStore.h"
#include <SD.h>

SDStore::SDStore(String fname, int csPin, void(*FUNC_DEBUG) (String)) {
  this->DEBUG = FUNC_DEBUG;
  fname.toCharArray(filename, MAX_FILENAME_SIZE);
  if (!SD.begin(csPin)) {
    DEBUG("Card failed, or not present");
  }
};

int SDStore::store(StoreEntry *entry) {
  dataFile = SD.open(filename, FILE_WRITE);
  if(dataFile) {
    entry->toJSON().printTo(dataFile);
    dataFile.print("\n");
    dataFile.close();
    return 0;
  } else {
    DEBUG("Card failed, or not present");
    return 1;
  }

};
