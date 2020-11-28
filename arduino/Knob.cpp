#include "Knob.h"

Knob::Knob(
  int minInputVal,
  int maxInputVal,
  byte pin
) {
  _minInputVal = minInputVal;
  _maxInputVal = maxInputVal;
  _pin = pin;

  pinMode(_pin, INPUT);
}

float readTargetPosRatio() {
  const int inputVal = analogRead(_pin);
  if (inputVal < _minInputVal) {
    return 0;
  }
  if (inputVal > _maxInputVal) {
    return 1;
  }
  return (float)(inputVal - _minInputVal) / (float)(_maxInputVal - _minInputVal);
}
