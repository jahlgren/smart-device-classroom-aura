#include "AirQualitySensor.h"

AQSensor airQualitySensor;

AQSensor::AQSensor() { }

bool AQSensor::begin(void) {
  int initVoltage = analogRead(PIN);

  for(int i = 0; i < READINGS_COUNT; i++) {
    readings[i] = -1;
  }

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

  int avg = 0;
  for(int i = 0; i < READINGS_COUNT; i++) {
    if(readings[i] < 0) return -1;
    avg += readings[i];
  }
  avg /= READINGS_COUNT;
  return avg;
}

void AQSensor::update() {
  unsigned long time = millis();
  unsigned long dt = time - _lastReadingTime;

  if(dt < READINGS_INTERVAL)
    return;

  _lastReadingTime = time;

  for(int i = 0; i < READINGS_COUNT-1; i++) {
    readings[i] = readings[i+1];
  }
  
  int value  = analogRead(PIN);
  readings[READINGS_COUNT - 1] = value;
}
