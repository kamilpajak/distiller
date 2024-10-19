#ifndef LCD_H
#define LCD_H

#include <Wire.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>

#define MULTIPLEXER_ADDRESS 0x70

class Lcd {
private:
  hd44780_I2Cexp lcd; // LCD object
  uint8_t channel;    // I2C multiplexer channel
  int lcdCols, lcdRows;

public:
  // Constructor: stores configuration but doesn't initialize LCD
  Lcd(int lcdCols, int lcdRows, uint8_t channel) : channel(channel), lcdCols(lcdCols), lcdRows(lcdRows) {}

  // Initializes the LCD; must be called after I2C is set up
  void init() {
    selectChannel(channel);
    lcd.begin(lcdCols, lcdRows);
  }

  // Displays text on a specific row
  void writeToRow(const String &text, int row) {
    selectChannel(channel);
    lcd.setCursor(0, row);
    lcd.print(text.c_str());
  }

  // Clears the display
  void clear() {
    selectChannel(channel);
    lcd.clear();
  }

private:
  // Selects the correct I2C channel on the multiplexer
  void selectChannel(uint8_t channel) {
    Wire.beginTransmission(MULTIPLEXER_ADDRESS);
    Wire.write(1 << channel);
    Wire.endTransmission();
    delay(100); // Ensure channel switching
  }
};

#endif // LCD_H
