#include "Alarm.h"

Alarm alarm;

Alarm::Alarm() { }

void Alarm::begin() {
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
}

void Alarm::on(unsigned long beepDurationMs, unsigned long intervalMs) {
  if (_isOn &&
      _beepDuration == beepDurationMs &&
      _interval == intervalMs) return;

  _isOn = true;
  _beepDuration = beepDurationMs;
  _interval = intervalMs;

  _lastChange = millis();
  _buzzerState = true;  // <- start with a beep
  digitalWrite(BUZZER_PIN, HIGH);
}

void Alarm::off() {
  if (!_isOn) return;

  _isOn = false;
  _buzzerState = false;
  digitalWrite(BUZZER_PIN, LOW);
}

void Alarm::update() {
  if (!_isOn) return;

  unsigned long now = millis();

  if (_buzzerState) {
    // Currently beeping
    if (now - _lastChange >= _beepDuration) {
      _buzzerState = false;
      digitalWrite(BUZZER_PIN, LOW);
      _lastChange = now;
    }
  } else {
    // Currently silent
    if (now - _lastChange >= _interval) {
      _buzzerState = true;
      digitalWrite(BUZZER_PIN, HIGH);
      _lastChange = now;
    }
  }
}
