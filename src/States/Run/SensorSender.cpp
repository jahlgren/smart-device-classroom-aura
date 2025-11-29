#include "SensorSender.h"

SensorSender::SensorSender(WiFiClient& wifiClient)
: _wifiClient(wifiClient),
  _httpClient(nullptr),
  _lastSentValue(0),
  _lastSendTime(0)
{
    
}

void SensorSender::begin(const char* deviceId,
                         const char* readingType,
                         unsigned long sendIntervalMs,
                         int deltaThreshold)
{
  _host = "ec2-51-20-132-192.eu-north-1.compute.amazonaws.com";
  _port = 3000;

  char endpointBuffer[64];
  snprintf(endpointBuffer, sizeof(endpointBuffer),
           "/api/devices/%s/readings", deviceId);

  _endpoint = strdup(endpointBuffer);

  _readingType = readingType;
  _sendInterval = sendIntervalMs;
  _deltaThreshold = deltaThreshold;
  _lastSentValue = 0;
  _lastSendTime = 0;

  _httpClient = new HttpClient(_wifiClient, _host, _port);
}

void SensorSender::end() {
  if (_httpClient != nullptr) {
    delete _httpClient;
    _httpClient = nullptr;
  }
}

void SensorSender::update(int currentValue) {
  unsigned long now = millis();

  int elapsedSinceLast = now - _lastSendTime;
  bool timeElapsed = (elapsedSinceLast >= _sendInterval);

  // wait atleast 5 sec before sending.
  // this should be configurable..
  bool deltaExceeded = elapsedSinceLast > 5000 && (abs(currentValue - _lastSentValue) >= _deltaThreshold);

  if (timeElapsed || deltaExceeded) {
    sendToServer(currentValue);
    _lastSentValue = currentValue;
    _lastSendTime = now;
  }
}

void SensorSender::sendToServer(int value) {
  String json = "{";
  json += "\"value\": " + String(value) + ",";
  json += "\"type\": \"" + String(_readingType) + "\"";
  json += "}";

  Serial.print("Sending to: ");
  Serial.println(_endpoint);

  Serial.print("Data: ");
  Serial.println(json);


  _httpClient->beginRequest();
  _httpClient->post(_endpoint);
  _httpClient->sendHeader("Content-Type", "application/json");
  _httpClient->sendHeader("Content-Length", json.length());
  _httpClient->beginBody();
  _httpClient->print(json);
  _httpClient->endRequest();

  // The following code, which is commented, reads the response from the server.
  // We could use this to get information if this sensor reading is good or bad.
  // However, this code is blocking, therefore for now, it is commented just
  // to show how it would be possible to get information from server to the device.

  /*
  int statusCode = _httpClient->responseStatusCode();
  String response = _httpClient->responseBody();

  Serial.print("HTTP POST status: ");
  Serial.println(statusCode);

  Serial.print("Response: ");
  Serial.println(response);
  */
}