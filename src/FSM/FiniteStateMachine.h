#ifndef FINITESTATEMACINE_H
#define FINITESTATEMACINE_H

#include "State.h"

class FiniteStateMachine {
  private:
    State* current = nullptr;

  public:
    void setState(State* s) {
      if (current) current->exit();
      current = s;
      if (current) {
        current->enter();
      }
    }

    void update() {
      if (!current) return;

      current->update();

      State* next = current->nextState();
      if (next && next != current) {
        setState(next);
      }
    }

    State* getCurrentState() { return current; }
};

#endif