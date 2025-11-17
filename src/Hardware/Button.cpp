#include "Button.h"

Button button;

Button::Button() { }

void Button::begin() {
  btn.attach(BUTTON_PIN, INPUT_PULLUP);
  btn.interval(25);
}

void Button::update() {
  btn.update();
}

bool Button::isPressed() {

  btn.update();
  bool pressed = btn.read() == HIGH;

  return pressed;
}