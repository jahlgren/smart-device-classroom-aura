#ifndef RUNSTATE_H
#define RUNSTATE_H

#include "../../FSM/State.h"
#include "../../Config.h"
#include <WiFiNINA.h>

class RunState : public State {
  private:
    enum class SubState {
      None = 0,
      Failed,
      ConnectingToWiFi,
      Run
    };

    static const int TICK_RATE = 1000;
    
    SubState subState = SubState::None;
    SubState prevSubState = SubState::None;
    
    ConfigData configData;
    
    int  wiFiStatus;
    int  wiFiConnectionAttempt = 0;
    long lastWiFiUpdateTime;

    long lastTickTime;

    bool finished = false;

    //Buzzer functionality
    static constexpr uint8_t BUZZER_PIN = 5;
    int lastAirQualityState = -1; // -1 = unknown, 0 = good, 1 = moderate, 2 = bad
    
    static const char* BACKEND_URL;
    static const int BACKEND_PORT = 80;
    WiFiClient wifiClient;
    long lastSendTime = 0;
    static const long SEND_INTERVAL = 30000; // Send data every 30 seconds

    void onSubStateChanged();
    void onFailedEnter();
    void onConnectingToWiFiEnter();
    void onConnectingToWiFiUpdate();
    void onRunEnter();
    void onRunUpdate();

    void checkSituationChange(int airQuality);
    void playSituationChangeSound(int newState);
    void sendToBackend(int airQuality);

  public:
    RunState();
    
    void enter() override;
    void update() override;
    void exit() override;
    State* nextState() override;
};

#endif