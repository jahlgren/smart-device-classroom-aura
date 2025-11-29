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
    
    Bounce btn;
};

extern Button button;

#endif