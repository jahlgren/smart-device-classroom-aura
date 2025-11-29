#ifndef SENSOR_SENDER_H
#define SENSOR_SENDER_H

#include <Arduino.h>
#include <WiFiNINA.h>
#include <ArduinoHttpClient.h>

class SensorSender {
public:
  SensorSender(WiFiClient& wifiClient);

  void begin(const char* deviceId,
             const char* readingType,
             unsigned long sendIntervalMs = 20000, // 2 min
             int deltaThreshold = 20);

  void end();
  void update(int currentValue);

private:
  WiFiClient& _wifiClient;
  HttpClient* _httpClient;

  const char* _host;
  uint16_t _port;
  const char* _endpoint;
  const char* _readingType;

  unsigned long _sendInterval;
  int _deltaThreshold;

  int _lastSentValue;

  unsigned long _lastSendTime;

  void sendToServer(int value);
};

#endif