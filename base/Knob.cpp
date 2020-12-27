#include "Knob.h"

Knob::Knob(
  int foldedInputVal,
  int unfoldedInputVal,
  byte pin,
  int noise
) {
  _foldedInputVal = foldedInputVal;
  _unfoldedInputVal = unfoldedInputVal;
  // float pour les divisions plus bas
  _inputValScale = abs(_unfoldedInputVal - _foldedInputVal);
  _pin = pin;
  _noise = noise;

  pinMode(_pin, INPUT);
}

bool Knob::_isFoldedVal(int inputVal) {
  if (_foldedInputVal < _unfoldedInputVal) {
    return inputVal <= (_foldedInputVal + _noise);
  }
  return inputVal >= (_foldedInputVal - _noise);
}

bool Knob::_isUnfoldedVal(int inputVal) {
  if (_foldedInputVal < _unfoldedInputVal) {
    return inputVal >= (_unfoldedInputVal - _noise);
  }
  return inputVal <= (_unfoldedInputVal + _noise);
}

int Knob::readTargetLen() {
  const int inputVal = analogRead(_pin);
 
  // On ignore le bruit
  if (_prevInputVal != -1 && abs(_prevInputVal - inputVal) <= _noise) {
    return NO_TARGET_LEN_CHANGE;
  }
  _prevInputVal = inputVal;

  if (_isFoldedVal(inputVal)) {
    return 0;
  }
  if (_isUnfoldedVal(inputVal)) {
    return MAX_TARGET_LEN;
  }
  float deltaToFolded = abs(inputVal - _foldedInputVal);
  return deltaToFolded / _inputValScale * MAX_TARGET_LEN;
}
