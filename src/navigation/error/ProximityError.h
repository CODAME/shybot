#include "store/StoreEntry.h"
#include "navigation/Navigator.h"

class ProximityError {

  public:
    static Navigator::status check(StoreEntry *entry, Navigator *navigator);
};
