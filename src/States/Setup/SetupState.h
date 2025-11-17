#ifndef SETUPSTATE_H
#define SETUPSTATE_H

#include <SPI.h>
#include <WiFiNINA.h>
#include "../../FSM/State.h"
#include "../../Config.h"

class SetupState : public State {
  private:
    enum class SubState {
      None = 0,
      Failed,
      Step1,
      Step2
    };

    SubState subState = SubState::None;
    SubState prevSubState = SubState::None;

    bool finished = false;

    int apStatus;
    WiFiServer* server;

    bool startServer(const char* ssid, const char* password);
    void stopServer();

    void onSubStateChanged();
    void onFailedEnter();
    void onStep1Enter();
    void onStep1Update();
    void onStep2Enter();
    void onStep2Update();

  public:
    SetupState();
    
    void enter() override;
    void update() override;
    void exit() override;
    State* nextState() override;
};

#endif