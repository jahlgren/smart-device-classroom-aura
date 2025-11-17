#ifndef STATE_H
#define STATE_H

class FiniteStateMachine;

class State {
  public:
    State() {}
    virtual void enter() {}
    virtual void update() {}
    virtual void exit() {}
    virtual State* nextState() { return nullptr; }
    virtual ~State() {}
};

#endif