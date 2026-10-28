#pragma once

#include <Arduino.h>

/**
 * @brief Controls the onboard status LED on the ESP32.
 * 
 * Manages digital output state, providing high-level commands
 * to turn on, turn off, or toggle the LED based on incoming MQTT messages.
 */
class LedController {
public:
  explicit LedController(uint8_t pin = 2);
  ~LedController();

  void begin();
  void turnOn();
  void turnOff();
  void toggle();
  void setState(bool is_on);
  bool isOn() const;

private:
  uint8_t _pin;
  bool _is_on;
};
