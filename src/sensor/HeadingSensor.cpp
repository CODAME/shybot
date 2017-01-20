#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>

#include "helpers.h"
#include "HeadingSensor.h"

HeadingSensor::HeadingSensor() {
  this->mag = Adafruit_HMC5883_Unified(12345);
  if(!this->mag.begin()) {
     DEBUG("No sensor found.");
  }
}

void HeadingSensor::applyDeclination(float &heading) {
  float declinationAngle = 0.22;
  heading += declinationAngle;
  if(heading < 0)
    heading += 2*PI;

  // Check for wrap due to addition of declination.
  if(heading > 2*PI)
    heading -= 2*PI;
}

HeadingSensor::Heading HeadingSensor::getHeading() {
  sensors_event_t event;
  this->mag.getEvent(&event);
  float heading = atan2(event.magnetic.y, event.magnetic.x);
  this->applyDeclination(heading);

  // Convert radians to degrees for readability.
  return Heading({ (float) (heading * 180/M_PI) });
}
