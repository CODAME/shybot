#include <Arduino.h>
#include <SD.h>

#include "SDStore.h"
#include "helpers.h"


const char* failMsg = "Card failed, or not present";
char databuf[512];


SDStore::SDStore(String fname, int csPin) {
  fname.toCharArray(filename, MAX_FILENAME_SIZE);
  if (!SD.begin(csPin)) {
    DEBUG(failMsg);
  }
};

int SDStore::store(StoreEntry *entry) {
  dataFile = SD.open(filename, FILE_WRITE);
  if(dataFile) {
    if (dataFile.size() == 0) {
      dataFile.println(entry->getCSVHeaders());
    }

    dataFile.println(entry->getCSV());
    dataFile.close();
    return 0;
  } else {
    DEBUG(failMsg);
    return 1;
  }

};
