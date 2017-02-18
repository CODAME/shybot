#include "ProximitySensor.h"
#include "helpers.h"

ProximitySensor::ProximitySensor(MCP3008 *adc,
                                 sensor_orientation myOrientation
                                ) {
  _adc = adc;
  orientation = myOrientation;
};

void ProximitySensor::getProximity(Proximity *proximity) {
  uint32_t distance = constrain(_adc->readADC(getChannel()) * 5, 0, 2000);
  proximity->orientation = orientation;
  proximity->distance = distance;
};
int ProximitySensor::getChannel() {
  return adc_pin[orientation];
}
