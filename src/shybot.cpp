#include <Arduino.h>
#include <Wire.h>

#include "sensor/Heading.h"
#include "store/SDStore.h"

#define PIN_CS 4

/* Assign a unique ID to this sensor at the same time */
String dataString;
float headingDegrees;
Heading *heading;
StoreEntry *storeEntry;
SDStore *sdStore;

void DEBUG(String err) {
  Serial.println(err);
}
void setup(void)
{
  Serial.begin(9600);
  while(!Serial) {
  }
  heading = new Heading(DEBUG);
  storeEntry = new StoreEntry(DEBUG);
  sdStore = new SDStore("readings.txt", PIN_CS, DEBUG);
}

void loop(void)
{
  storeEntry->setHeading(heading->getHeadingDegrees());
  //storeEntry->log();
  sdStore->store(storeEntry);

  delay(500);
}
