#include "AirQualitySensor.h"

AQSensor airQualitySensor;

AQSensor::AQSensor() { }

bool AQSensor::begin(void) {
  int initVoltage = analogRead(PIN);

  if (10 < initVoltage && initVoltage < 798) {
    return true;
  } else {
    return false;
  }
}

int AQSensor::get(void) {

  // Rough estimate of the air quality.
  // This is not accurate. For a real product, a proper sensor is required.
  // Higher value means worse air quality.

  int value  = analogRead(PIN);
  return value;
}
