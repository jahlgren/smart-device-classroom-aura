#include "RunState.h"
#include <SPI.h>
#include <WiFiNINA.h>

#include "../../Hardware/Display.h"
#include "../../Hardware/AirQualitySensor.h"
#include <ArduinoHttpClient.h>

// Backend server URL
const char* RunState::BACKEND_URL = "our-backend.com";

RunState::RunState() : State() {}

void RunState::enter() {
  Serial.println("RunState: Enter");

  // Initialize buzzer pin
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  
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
  lastSendTime = 0;
}

void RunState::onRunUpdate() {
  long time = millis();
  if(time - TICK_RATE >= lastTickTime) {
    lastTickTime = time;

    int airQuality = airQualitySensor.get();

    // Update screen color based on air quality thresholds
    
    // 0-150 = Good, 151-200 = Moderate, 201+ = Bad
    
    if (airQuality <= 150) {
      display.setBg(Display::Color::Default);  // Good - White
    } else if (airQuality <= 200) {
      display.setBg(Display::Color::Warning);  // Moderate - Orange
    } else {
      display.setBg(Display::Color::Error);    // Bad - Red
    }
    
    checkSituationChange(airQuality);
    
    // Send data to backend every 30 seconds 
    if (time - lastSendTime >= SEND_INTERVAL) {
        sendToBackend(airQuality);
        lastSendTime = time;
    }

    // Update display 
    display.setText(0, "Air Quality");
    char buffer[] = "               ";
    snprintf(buffer, sizeof(buffer), "%d", airQuality);
    display.setText(1, buffer);
  }
}

// Check if air quality situation has changed and trigger buzzer
// -1 = unknown, 0 = good, 1 = moderate, 2 = bad
void RunState::checkSituationChange(int airQuality) {

  int currentState;
    if (airQuality <= 150) {
        currentState = 0; // Good (0-150)
    } else if (airQuality <= 200) {
        currentState = 1; // Moderate (151-200)
    } else {
        currentState = 2; // Bad (201+)
    }
    
    // Check if state changed from previous reading
    if (currentState != lastAirQualityState) {
        playSituationChangeSound(currentState);
        lastAirQualityState = currentState;
        
        Serial.print("Situation changed: ");
        Serial.print(airQuality);
        Serial.print(" -> ");
        Serial.println(currentState == 0 ? "GOOD" : currentState == 1 ? "MODERATE" : "BAD");
    }
}

// Play different buzzer sounds based on air quality situation
// 0 = good, 1 = moderate, 2 = bad
void RunState::playSituationChangeSound(int newState) {
    switch(newState) {
        case 0: // Good (0-150) - High pitched short beep
            tone(BUZZER_PIN, 1500, 200);
            Serial.println("Air Quality: GOOD");
            break;
        case 1: // Moderate (151-200) - Medium pitched beep
            tone(BUZZER_PIN, 1000, 400);
            Serial.println("Air Quality: MODERATE");
            break;
        case 2: // Bad (201+) - Low pitched long beep
            tone(BUZZER_PIN, 600, 800);
            Serial.println("Air Quality: BAD");
            break;
    }
}

// Send air quality data to backend server by HTTP POST
void RunState::sendToBackend(int airQuality) {

  if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Cannot send: WiFi not connected");
        return;
    }
    
    // Create HTTP client for backend communication
    HttpClient client = HttpClient(wifiClient, BACKEND_URL, BACKEND_PORT);
    
    String contentType = "application/json";
    String postData = "{";
    postData += "\"deviceId\":\"" + String(config.data.deviceId) + "\",";
    postData += "\"airQuality\":" + String(airQuality) + ",";
    postData += "\"timestamp\":" + String(millis());
    postData += "}";
    
    Serial.println("Sending to backend: " + postData);
    
    client.post("/api/sensor-data", contentType, postData);
    
    // Read and log from backend
    int statusCode = client.responseStatusCode();
    String response = client.responseBody();
    
    Serial.print("Backend response: ");
    Serial.print(statusCode);
    Serial.println(" - " + response);
}