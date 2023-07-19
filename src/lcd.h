#ifndef LCD_H
#define LCD_H

#include <Wire.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>

#define MULTIPLEXER_ADDRESS 0x70

class Lcd {
private:
  hd44780_I2Cexp lcd;
  uint8_t channel;

public:
  /**
   * Constructor for the LCDController class.
   * @param lcdCols The number of columns of the LCD display.
   * @param lcdRows The number of rows of the LCD display.
   * @param channel The channel number on the multiplexer.
   */
  Lcd(int lcdCols, int lcdRows, uint8_t channel) : channel(channel) {
    selectChannel(channel);
    lcd.begin(lcdCols, lcdRows);
  }

  /**
   * Displays text on the LCD display at the specified row.
   * @param text The text to be displayed.
   * @param row The row index where the text should be displayed.
   */
  void displayText(String text, int row) {
    lcd.setCursor(0, row);
    lcd.print(text.c_str());
  }

  /**
   * Clears the LCD display.
   */
  void clear() { lcd.clear(); }

  void selectChannel(uint8_t channel) {
    Wire.beginTransmission(MULTIPLEXER_ADDRESS);
    Wire.write(1 << channel);
    Wire.endTransmission();
  }
};

#endif // LCD_H
