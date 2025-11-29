#include "src/Config.h"
#include "src/Hardware/Display.h"
#include "src/Hardware/Button.h"
#include "src/Hardware/Alarm.h"
#include "src/Hardware/AirQualitySensor.h"
#include "src/FSM/FiniteStateMachine.h"
#include "src/States/Boot/BootState.h"
#include "src/States/Setup/SetupState.h"
#include "src/States/Run/RunState.h"

BootState  bootState;
SetupState setupState;
RunState   runState;

FiniteStateMachine fsm;

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(0));
  
  display.begin();
  button.begin();
  alarm.begin();
  airQualitySensor.begin();

  bootState.setNextStates(&setupState, &runState);
  fsm.setState(&bootState);
}

void loop() {
  display.update();
  button.update();
  alarm.update();
  airQualitySensor.update();
  fsm.update();
}
