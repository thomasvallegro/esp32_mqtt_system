#include "TemperatureSensor.h"

TemperatureSensor::TemperatureSensor()
  : _last_reading_celsius(0.0f) {
}

TemperatureSensor::~TemperatureSensor() {
}

float TemperatureSensor::readCelsius() {
  // Lettura del sensore termico interno al silicio tramite API nativa ESP32
  _last_reading_celsius = temperatureRead();
  return _last_reading_celsius;
}

float TemperatureSensor::readFahrenheit() {
  float celsius = readCelsius();
  return (celsius * 1.8f) + 32.0f;
}
