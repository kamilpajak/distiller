#ifndef RELAY_H
#define RELAY_H

/**
 * Class for controlling a relay.
 */
class Relay {
private:
  int pin;   /**< The pin number for the relay. */
  bool isOn; /**< The state of the relay. */

public:
  /**
   * Constructor for the Relay class.
   * @param pin The pin number for the relay.
   */
  explicit Relay(int pin) : pin(pin), isOn(false) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
  }

  /**
   * Turns on the relay if it's not already on.
   */
  void turnOn() {
    if (!isOn) {
      digitalWrite(pin, HIGH);
      isOn = true;
    }
  }

  /**
   * Turns off the relay if it's not already off.
   */
  void turnOff() {
    if (isOn) {
      digitalWrite(pin, LOW);
      isOn = false;
    }
  }
};

#endif // RELAY_H
