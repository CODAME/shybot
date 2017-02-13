#ifndef SB_LOGSTORE
#define SB_LOGSTORE

#include <Arduino.h>
#include "StoreEntry.h"


class LogStore {
  public:
    LogStore();

    int store(StoreEntry *entry);

    int graph(StoreEntry *entry);

};
#endif
