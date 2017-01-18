#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>

#include "Heading.h"

void(*DEBUG) (String);

Heading::Heading(void(*FUNC_DEBUG) (String)) {
  DEBUG = FUNC_DEBUG;
  this->mag = Adafruit_HMC5883_Unified(12345);
  if(!this->mag.begin()) {
     DEBUG("No sensor found.");
  }
}

void Heading::applyDeclination(float &heading) {
  float declinationAngle = 0.22;
  heading += declinationAngle;
  if(heading < 0)
    heading += 2*PI;

  // Check for wrap due to addition of declination.
  if(heading > 2*PI)
    heading -= 2*PI;
}

float Heading::getHeadingDegrees() {
  sensors_event_t event;
  this->mag.getEvent(&event);
  float heading = atan2(event.magnetic.y, event.magnetic.x);
  this->applyDeclination(heading);

  // Convert radians to degrees for readability.
  return heading * 180/M_PI;
}
