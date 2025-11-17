#ifndef DISPLAY_H
#define DISPLAY_H

#include <Wire.h>
#include "rgb_lcd.h"

class Display {
  public:
    Display();
    
    enum class Color {
        Default,
        Dim,
        Info,
        Warning,
        Error
    };

    void begin();
    void update();

    void setBg(uint8_t r, uint8_t g, uint8_t b);
    void setBg(Color color);
    void setText(uint8_t row, const char* text);
    void clear();

  private:
    static const uint8_t ROWS = 2;
    static const uint8_t COLS = 16;
    static const uint8_t MAX_LENGTH = 128;
    static const uint8_t SCROLL_PADDING = 4;
    static const unsigned long SCROLL_INTERVAL = 500;

    rgb_lcd lcd;
    char rowBuffer[ROWS][MAX_LENGTH + SCROLL_PADDING + 1]; // +1 for \0
    char displayBuffer[ROWS][COLS + 1];   // +1 for \0
    uint8_t rowCursor[ROWS];

    unsigned long lastScrollTime;
    
    void scrollRow(uint8_t row);
};

extern Display display;

#endif