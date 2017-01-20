#include <Arduino.h>
#include <SD.h>

#include "helpers.h"
#include "StoreEntry.h"

#define MAX_FILENAME_SIZE 256

class SDStore {
  public:
    SDStore(String fname, int csPin);

    int store(StoreEntry* entry);

  private:
    char filename[MAX_FILENAME_SIZE];
    File dataFile;
    void(*DEBUG) (String);
};
