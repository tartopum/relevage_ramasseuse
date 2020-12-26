#include "Knob.h"

Knob::Knob(
  int foldedInputVal,
  int unfoldedInputVal,
  byte pin
) {
  _foldedInputVal = foldedInputVal;
  _unfoldedInputVal = unfoldedInputVal;
  _pin = pin;

  pinMode(_pin, INPUT);
}

int Knob::readTargetLen() {
  /*
   * Selon les branchements du potentiometre, les valeurs basses du capteur
   * correspondent a une position repliee ou depliee du verin.
   */
  const int inputVal = analogRead(_pin);
 
  // float pour les divisions plus bas
  float scale = abs(_unfoldedInputVal - _foldedInputVal);

  if (_foldedInputVal < _unfoldedInputVal) {
    // Les valeurs basses du potentiometre correspondent a un verin replie.
    if (inputVal <= _foldedInputVal) {
      return 0;
    }
    if (inputVal >= _unfoldedInputVal) {
      return 1000;
    }
    const float distToFolded = inputVal - _foldedInputVal;
    return distToFolded / scale * 1000;
  }

  // Les valeurs hautes du potentiometre correspondent a un verin replie.
  if (inputVal >= _foldedInputVal) {
    return 0;
  }
  if (inputVal <= _unfoldedInputVal) {
    return 1000;
  }
  const float distToUnfolded = inputVal - _unfoldedInputVal;
  return (1 - distToUnfolded / scale) * 1000;
}
