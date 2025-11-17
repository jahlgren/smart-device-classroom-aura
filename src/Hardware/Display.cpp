#include "Display.h"

Display display;

Display::Display() { }

void Display::begin() {
  lcd.begin(COLS, ROWS);
  clear();
  setBg(255, 255, 255);
}

void Display::update() {
  unsigned long currentTime = millis();
  if (currentTime - lastScrollTime >= SCROLL_INTERVAL) {
    lastScrollTime = currentTime;
    for(uint8_t i = 0; i < ROWS; i++) {
      scrollRow(i);
    }
  }
}

void Display::setBg(uint8_t r, uint8_t g, uint8_t b) {
  lcd.setRGB(r, g, b);
}

void Display::setBg(Color color) {
  switch(color) {
    case Color::Default:
      lcd.setRGB(255, 255, 255);
      break;
    case Color::Dim:
      lcd.setRGB(128, 128, 128);
      break;
    case Color::Info:
      lcd.setRGB(0, 90, 255);
      break;
    case Color::Warning:
      lcd.setRGB(255, 165, 0);
      break;
    case Color::Error:
      lcd.setRGB(255, 0, 0);
      break;
  }
}

void Display::setText(uint8_t row, const char* text) {
  if(row >= ROWS) return;
  
  lcd.setCursor(0, row);
  char clearBuffer[COLS];
  for(int i = 0; i < COLS; i++) {
    clearBuffer[i] = ' ';
  }
  clearBuffer[COLS-1] = '\0';
  lcd.print(clearBuffer);

  uint8_t pos = 0;
  for(pos = 0; pos < MAX_LENGTH; pos++) {
    if(text[pos] == '\0') break;
    rowBuffer[row][pos] = text[pos];
  }
  for(uint8_t i = 0; i < SCROLL_PADDING; i++, pos++) {
    rowBuffer[row][pos] = ' ';
  }
  rowBuffer[row][pos] = '\0';

  strncpy(displayBuffer[row], text, COLS);
  displayBuffer[row][COLS] = '\0';
  
  rowCursor[row] = 0;

  lcd.setCursor(0, row);
  lcd.print(displayBuffer[row]);
  
  lastScrollTime = millis();
}

void Display::clear() {
  lcd.clear();
  
  for(uint8_t i = 0; i < ROWS; i++) {
    rowBuffer[i][0] = '\0';
    rowCursor[i] = 0;
  }

  lastScrollTime = millis();
}

void Display::scrollRow(uint8_t row) {
  uint8_t length = strlen(rowBuffer[row]);

  if(length - SCROLL_PADDING < COLS) return;

  rowCursor[row]++;
  if(rowCursor[row] >= length + SCROLL_PADDING) rowCursor[row] = 0;
  uint8_t cursor = rowCursor[row];

  for(uint8_t i = 0; i < COLS; i++) {
    displayBuffer[row][i] = rowBuffer[row][(cursor + i) % length];
  }
  displayBuffer[row][COLS] = '\0';

  lcd.setCursor(0, row);
  lcd.print(displayBuffer[row]);
}
