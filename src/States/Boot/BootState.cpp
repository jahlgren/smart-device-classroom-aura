#include "BootState.h"
#include "../../Hardware/Display.h"
#include "../../Hardware/Button.h"

BootState::BootState() : State() {}

void BootState::setNextStates(State* setup, State* run) {
  setupState = setup;
  runState = run;
}

void BootState::enter() {
  Serial.println("BootState: Enter");

  display.clear();
  display.setBg(Display::Color::Default);
  display.setText(0, "Starting");
  startTime = millis();
  finished = false;
}

void BootState::update() {
  // Wait so there is time to press the button to enter setup mode.
  if (!finished && millis() - startTime >= 5000) {
    finished = true;

    next = button.isPressed() ? setupState : runState;
  }
}

void BootState::exit() {
  Serial.println("BootState: Exit");
  display.clear();
}

State* BootState::nextState() {
  return finished ? next : nullptr;
}