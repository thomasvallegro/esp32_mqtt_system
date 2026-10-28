#pragma once

#include <Arduino.h>

/**
 * @brief Reads and processes the internal CPU core temperature of the ESP32.
 * 
 * Uses the built-in on-die temperature sensor of the ESP32 chip
 * to provide thermal readings in Celsius and Fahrenheit without external sensors.
 */
class TemperatureSensor {
public:
  TemperatureSensor();
  ~TemperatureSensor();

  float readCelsius();
  float readFahrenheit();

private:
  float _last_reading_celsius;
};
