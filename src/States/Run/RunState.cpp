#include <SPI.h>
#include <WiFiNINA.h>

#include "RunState.h"
#include "../../Hardware/Display.h"
#include "../../Hardware/AirQualitySensor.h"

RunState::RunState() : State() {}

void RunState::enter() {
  Serial.println("RunState: Enter");

  display.clear();
  display.setBg(Display::Color::Dim);
  
  if(!config.load()) {
    Serial.println("Failed to load config!");
    subState = SubState::Failed;
  }
  else {
    subState = SubState::ConnectingToWiFi;
  }

  finished = false;
}

void RunState::update() {
  if(prevSubState != subState) {
    prevSubState = subState;
    onSubStateChanged();
  }

  switch(subState) {
    case SubState::ConnectingToWiFi: return onConnectingToWiFiUpdate();
    case SubState::Run: return onRunUpdate();
  }
}

void RunState::exit() {
  Serial.println("RunState: Exit");
  display.clear();
}

State* RunState::nextState() {
  return finished ? nullptr : nullptr;
}

void RunState::onSubStateChanged() {
  switch(subState) {
    case SubState::Failed: return onFailedEnter();
    case SubState::ConnectingToWiFi: return onConnectingToWiFiEnter();
    case SubState::Run: return onRunEnter();

    default:
      Serial.println("SubState not recognized");
      return;
  }
}

void RunState::onFailedEnter() {
  Serial.println("Enter Failed!");
  display.clear();
  display.setBg(Display::Color::Error);
  display.setText(0, "Error");
  display.setText(1, "Something went wrong, please try again.");
}

void RunState::onConnectingToWiFiEnter() {
  Serial.println("Enter Connecting to WiFi!");
  display.clear();
  display.setBg(Display::Color::Dim);
  display.setText(0, "Connecting/WiFi");
  display.update();

  wiFiStatus = WiFi.status();
  if (wiFiStatus == WL_NO_MODULE) {
    Serial.println("WiFi module not found!");
    subState = SubState::Failed;
    return;
  }

  lastWiFiUpdateTime = 0;
  wiFiConnectionAttempt = 0;

  wiFiStatus = WL_IDLE_STATUS;
}

void RunState::onConnectingToWiFiUpdate() {
  long time = millis();
  char buffer[] = "Retrying       ";

  if(time - 3000 >= lastWiFiUpdateTime) {
    lastWiFiUpdateTime = time;

    if(wiFiConnectionAttempt > 3)
    {
      Serial.println("Failed to connect to WiFi.");
      subState = SubState::Failed;
      return;
    }
    else if(wiFiStatus != WL_CONNECTED) {
      wiFiConnectionAttempt++;

      Serial.print("Connecting to ");
      Serial.println(config.data.ssid);
      Serial.println(config.data.password);

      for(int i = 0; i < wiFiConnectionAttempt; i++) {
        buffer[8+i] = '.';
      }
      display.setText(1, buffer);
      
      if(strlen(config.data.ssid) < 1) {
      Serial.println("Failed to connect to WiFi. Invalid SSID.");
      subState = SubState::Failed;
        return;  
      }

      wiFiStatus = WiFi.begin(config.data.ssid, config.data.password);
    }
    else {
      // Connected !
      Serial.println("Connected to WiFi!");
      subState = SubState::Run;
    }
  }
}

void RunState::onRunEnter() {
  Serial.println("All OK! Device running normally.");
  display.clear();
  display.setBg(Display::Color::Dim);
  lastTickTime = 0;
}

void RunState::onRunUpdate() {
  long time = millis();
  if(time - TICK_RATE >= lastTickTime) {
    lastTickTime = time;

    int airQuality = airQualitySensor.get();

    display.setText(0, "Air Quality");
    
    char buffer[] = "               ";
    snprintf(buffer, sizeof(buffer), "%d", airQuality);
    display.setText(1, buffer);
  }
}
