#ifndef AIRQUALITYSENSOR_H
#define AIRQUALITYSENSOR_H

#include "Arduino.h"

class AQSensor {
  public:
    AQSensor();

    bool begin(void);
    int  get(void);

  private:
    static const int PIN = A0;
};

extern AQSensor airQualitySensor;

#endif