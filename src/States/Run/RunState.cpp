#include "RunState.h"
#include <SPI.h>
#include <WiFiNINA.h>

#include "../../Hardware/Display.h"
#include "../../Hardware/AirQualitySensor.h"
#include "../../Hardware/Alarm.h"

RunState::RunState() 
: airQualitySender(wifiClient),
  State()
{}

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

  if(time - 4000 >= lastWiFiUpdateTime) {
    lastWiFiUpdateTime = time;

    if(wiFiConnectionAttempt > 6)
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

      for(int i = 0; i < wiFiConnectionAttempt/2; i++) {
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
  lastAirQualityReading = -1;

  Serial.print("::: Device id: ");
  Serial.println(config.data.deviceId);
  
  airQualitySender.begin(
    config.data.deviceId,
    "airQuality"
  );
}

void RunState::onRunUpdate() {
  long time = millis();
  if(time - TICK_RATE >= lastTickTime) {
    
    int airQuality = airQualitySensor.get();
    if(airQuality < 0) {
      // This means that there is not enough readings..
      display.setText(0, "Air Quality");
      display.setText(0, "...");
      return;
    }
    
    lastTickTime = time;

    int wifiStatus = WiFi.status();
    if(wifiStatus != WL_CONNECTED) {
      Serial.println("WiFi connection lost!");
      subState = SubState::Failed;
      alarm.on(1000, 0);
      return;
    }

    airQualitySender.update(airQuality);

    if(airQuality == lastAirQualityReading)
      return;

    lastAirQualityReading = airQuality;

    if (airQuality <= 50) {             // Good
      alarm.off();
      display.setBg(0, 228, 0);           
    } else if (airQuality <= 100) {     // Moderate
      alarm.off();
      display.setBg(100, 225, 0);        
    } else if (airQuality <= 150) {     // Unhealthy for Sensitive Groups
      alarm.on(50, 10000);
      display.setBg(200, 50, 0);         
    } else if (airQuality <= 200) {     // Unhealthy
      alarm.on(300, 300);
      display.setBg(255, 0, 0);           
    } else if (airQuality <= 300) {     // Very Unhealthy
      alarm.on(200, 200);
      display.setBg(150, 0, 100);         
    } else {                            // Hazardous
      alarm.on(100, 100);
      display.setBg(200, 0, 55);          
    }

    // Update display 
    display.setText(0, "Air Quality");
    char buffer[] = "               ";
    snprintf(buffer, sizeof(buffer), "%d", airQuality);
    display.setText(1, buffer);
  }
}
