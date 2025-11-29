#ifndef AIRQUALITYSENSOR_H
#define AIRQUALITYSENSOR_H

#include "Arduino.h"

class AQSensor {
  public:
    AQSensor();

    bool begin(void);
    int  get(void);
    void update();

  private:
    static const int PIN = A0;
    static const int READINGS_INTERVAL = 200;
    static const int READINGS_COUNT = 25;
    
    unsigned long _lastReadingTime = 0;
    int readings[READINGS_COUNT];
};

extern AQSensor airQualitySensor;

#endif