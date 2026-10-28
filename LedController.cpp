#include "LedController.h"

LedController::LedController(uint8_t pin)
  : _pin(pin),
    _is_on(false) {
}

LedController::~LedController() {
  turnOff();
}

void LedController::begin() {
  pinMode(_pin, OUTPUT);
  turnOff();
}

void LedController::turnOn() {
  digitalWrite(_pin, HIGH);
  _is_on = true;
}

void LedController::turnOff() {
  digitalWrite(_pin, LOW);
  _is_on = false;
}

void LedController::toggle() {
  setState(!_is_on);
}

void LedController::setState(bool is_on) {
  if (is_on) {
    turnOn();
  } else {
    turnOff();
  }
}

bool LedController::isOn() const {
  return _is_on;
}
