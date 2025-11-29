#ifndef ALARM_H
#define ALARM_H

#include <Arduino.h>

class Alarm {
public:
  Alarm();

  void begin();
  void update();

  void on(unsigned long beepDurationMs = 100,
          unsigned long intervalMs = 1000);

  void off();

private:
  static constexpr uint8_t BUZZER_PIN = 3;

  bool _isOn = false;
  
  unsigned long _beepDuration = 100;
  unsigned long _interval = 1000;

  unsigned long _lastChange = 0;
  bool _buzzerState = false;
};

extern Alarm alarm;

#endif