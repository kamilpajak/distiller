#ifndef LCD_H
#define LCD_H

#ifndef UNIT_TEST
#include <Arduino.h>
#include <Wire.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>
#endif

#include <cstdint>

// Constants
constexpr uint8_t MULTIPLEXER_ADDRESS = 0x70;
constexpr uint8_t CHANNEL_SWITCH_DELAY_MS = 100;

class Lcd {
private:
#ifndef UNIT_TEST
  Hd44780I2Cexp lcd; // LCD object
#endif
  uint8_t channel; // I2C multiplexer channel
  int lcdCols, lcdRows;

public:
  // Constructor: stores configuration but doesn't initialize LCD
  Lcd(int lcdCols, int lcdRows, uint8_t channel) : channel(channel), lcdCols(lcdCols), lcdRows(lcdRows) {}

#ifndef UNIT_TEST
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
  static void selectChannel(uint8_t channel) {
    Wire.beginTransmission(MULTIPLEXER_ADDRESS);
    TwoWire::write(1 << channel);
    TwoWire::endTransmission();
    delay(CHANNEL_SWITCH_DELAY_MS); // Ensure channel switching
  }
#endif
};

#endif // LCD_H
