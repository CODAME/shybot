#include "ProximitySensor.h"
#include "helpers.h"

int lastTrigger = millis();
int triggerPin = -1;

ProximitySensor::ProximitySensor(MCP3008 *adc,
                                 sensor_orientation myOrientation
                                ) {
  _adc = adc;
  orientation = myOrientation;
};

void ProximitySensor::getProximity(Proximity *proximity) {
  uint32_t distance = constrain(_adc->readADC(getChannel()) * 5, 0, 2000);
  Proximity *newProx = new ProximitySensor::Proximity({ orientation, distance, millis() });
  readings.add(newProx);
  if(readings.size() > NUM_READINGS) {
    delete readings.shift();
  }
  proximity->orientation = orientation;
  proximity->distance = distance;
  proximity->trend = simpleSlope();
  proximity->stdDev = stdDev();
  proximity->avg = avg();
  newProx->stdDev = proximity->stdDev;
  newProx->avg = proximity->avg;
  newProx->trend = proximity->trend;
  newProx->delta = delta();
  proximity->delta = delta();
};

void ProximitySensor::clearHistory() {
  while(readings.size()) {
    delete readings.shift();
  }
}

int ProximitySensor::getChannel() {
  return adc_pin[orientation];
}

void ProximitySensor::setTriggerPin(int pin) {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
  triggerPin = pin;
}

void ProximitySensor::triggerIfNeeded() {
  if(triggerPin != -1 && (millis() - lastTrigger > SB_PROX_TRIGGER_INTERVAL_MS)) {
    lastTrigger = millis();
    digitalWrite(triggerPin, HIGH);
    delayMicroseconds(SB_PROX_TRIGGER_LEN_US);
    delay(10);
    digitalWrite(triggerPin, LOW);
    delay(SB_PROX_TRIGGER_INTERVAL_MS);
  }
}

double ProximitySensor::delta() {
  if (readings.size() < NUM_READINGS) {
    return 0;
  }
  double cur = readings.get(NUM_READINGS - 1)->distance;
  double prev = avg();
  return cur - prev;
}

double ProximitySensor::avg() {
  int size = readings.size();
  float sum = 0.0;
  for(int i=0; i<size; i++) {
    sum += readings.get(i)->distance;
  }
  return sum / size;
}

double ProximitySensor::stdDev() {
  int size = readings.size();
  float sum = 0.0, mean, standardDeviation = 0.0;

  for(int i=0; i<size; i++) {
    sum += readings.get(i)->distance;
  }
  mean = sum / size;

  for(int i=0; i<size; i++) {
    standardDeviation += pow(readings.get(i)->distance - mean, 2);
  }

  return sqrt(standardDeviation / size);
}

double ProximitySensor::simpleSlope() {
  if (readings.size() < NUM_READINGS) {
    return 0;
  }
  double last = readings.get(NUM_READINGS - 1)->distance;
  double first = readings.get(0)->distance;
  return (last - first) / NUM_READINGS;
}

double ProximitySensor::linRegSlope() {
  // pass x and y arrays (pointers), lrCoef pointer, and n.  The lrCoef array is comprised of the slope=lrCoef[0] and intercept=lrCoef[1].  n is length of the x and y arrays.
  // http://en.wikipedia.org/wiki/Simple_linear_regression

  // initialize variables
  float xbar=0;
  float ybar=0;
  float xybar=0;
  float xsqbar=0;
  float lrCoef[2] = {0, 0};

  int size = readings.size();
  if (size < 5 ) { return 0; }

  // calculations required for linear regression
  for (int i=0; i<size; i++){
    Proximity *p = readings.get(i);
    xbar = xbar + i;
    ybar = ybar + p->distance;
    xybar = xybar + i * p->distance;
    xsqbar = xsqbar + i * i;
  }
  xbar = xbar / size;
  ybar = ybar / size;
  xybar = xybar / size;
  xsqbar = xsqbar / size;

  // simple linear regression algorithm
  lrCoef[0]=(xybar-xbar*ybar)/(xsqbar-xbar*xbar);
  lrCoef[1]=ybar-lrCoef[0]*xbar;
  return lrCoef[0];
}
