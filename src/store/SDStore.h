#include <Arduino.h>
#include <SD.h>

#include "helpers.h"
#include "StoreEntry.h"

#define MAX_FILENAME_SIZE 256

class SDStore {
  public:
    SDStore(const char *fname, int csPin);

    int store(StoreEntry* entry);

  private:
    const char *filename;
    File dataFile;
};
