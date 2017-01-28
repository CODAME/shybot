#include "store/StoreEntry.h"
#include "navigation/Navigator.h"

#define MIN_RPM .02

class RPMError {
public:
  static Navigator::status check(StoreEntry *entry, Navigator *navigator);
};
