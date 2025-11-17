#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>
#include <Bounce2.h>

class Button {
  public:
    Button();

    void begin();
    void update();
    bool isPressed();

  private:
    static constexpr uint8_t BUTTON_PIN = 4;
    // static constexpr bool ACTIVE_LOW = false;
    
    Bounce btn;
    
    // static constexpr unsigned long DEBOUNCE_DELAY = 50; // ms



    // unsigned long _lastDebounceTime = 0;
    // bool _lastState = false;
    // bool _currentState = false;
};

extern Button button;

#endif