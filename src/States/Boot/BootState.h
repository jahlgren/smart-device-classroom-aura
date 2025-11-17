#ifndef BOOTSTATE_H
#define BOOTSTATE_H

#include <Arduino.h>
#include "../../FSM/State.h"

class BootState : public State {
  private:
    unsigned long startTime = 0;
    bool finished = false;

    State* setupState = nullptr;
    State* runState = nullptr;
    State* next = nullptr;

  public:
    BootState();
    
    void setNextStates(State* setup, State* run);

    void enter() override;
    void update() override;
    void exit() override;
    State* nextState() override;
};

#endif