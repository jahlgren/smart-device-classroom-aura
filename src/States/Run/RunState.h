#ifndef RUNSTATE_H
#define RUNSTATE_H

#include "../../FSM/State.h"
#include "../../Config.h"

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

    void onSubStateChanged();
    void onFailedEnter();
    void onConnectingToWiFiEnter();
    void onConnectingToWiFiUpdate();
    void onRunEnter();
    void onRunUpdate();

  public:
    RunState();
    
    void enter() override;
    void update() override;
    void exit() override;
    State* nextState() override;
};

#endif