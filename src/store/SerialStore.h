#include <Arduino.h>

#include "StoreEntry.h"


class SerialStore {
  public:
    SerialStore(void(*FUNC_DEBUG) (String));

    int store(StoreEntry* entry);

  private:
    void(*DEBUG) (String);
};
