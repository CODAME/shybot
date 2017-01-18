#include <Arduino.h>
#include <Wire.h>
#include <SD.h>

#include "sensor/Heading.h"
#include "store/StoreEntry.h"

#define PIN_CS 4

/* Assign a unique ID to this sensor at the same time */
String dataString;
File dataFile;
float headingDegrees;
Heading* heading;
StoreEntry* storeEntry;

void DEBUG(String err) {
  Serial.println(err);
}

void setup(void)
{
  Serial.begin(9600);
  heading = new Heading(DEBUG);
  storeEntry = new StoreEntry(DEBUG);

  /* Initialise the sensor */
  if (!SD.begin(PIN_CS)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
}

void loop(void)
{
  storeEntry->setHeading(heading->getHeadingDegrees());
  storeEntry->log();
  /*
  dataFile = SD.open("heading.txt", FILE_WRITE);
  if(dataFile) {
    dataFile.println(dataString);
    dataFile.close();
  } else {
    Serial.println("Error opening datalog.txt");
  }
  */

  delay(500);
}
