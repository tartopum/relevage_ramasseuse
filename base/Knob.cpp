#include "Knob.h"

Knob::Knob(
  float minInputVolts,
  float maxInputVolts,
  byte pin
) {
  _minInputVal = minInputVolts / 5 * 1023;
  _maxInputVal = maxInputVolts / 5 * 1023;
  _pin = pin;

  pinMode(_pin, INPUT);
}

float Knob::readTargetPosRatio() {
  const int inputVal = analogRead(_pin);
  if (inputVal < _minInputVal) {
    return 0;
  }
  if (inputVal > _maxInputVal) {
    return 1;
  }
  return (float)(inputVal - _minInputVal) / (float)(_maxInputVal - _minInputVal);
}
