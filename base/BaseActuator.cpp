#include "BaseActuator.h"

BaseActuator::BaseActuator(
  int posPerThousandAccuracy,
  float posInputMinVolts,
  float posInputMaxVolts,
  byte posInputPin
) {
    _posPerThousandAccuracy = posPerThousandAccuracy;
  _posInputMin = posInputMinVolts / 5 * 1023;
  _posInputMax = posInputMaxVolts / 5 * 1023;
  _posInputPin = posInputPin;

  pinMode(_posInputPin, INPUT);
}

int BaseActuator::_readPosPerThousand() {
  const int inputVal = analogRead(_posInputPin);
  const float posRatio = (float)(inputVal - _posInputMin) / (float)(_posInputMax - _posInputMin);
  return (int)(posRatio * 1000);
}

void BaseActuator::stepTo(int targetPerThousand) {
  int posDelta = targetPerThousand - _readPosPerThousand();
  bool isAtPos = abs(posDelta) < _posPerThousandAccuracy;
  bool cannotStep = (posDelta < 0 && _isTotallyFolded()) || (posDelta > 0 && _isTotallyUnfolded());
  if (isAtPos || cannotStep) {
    stop();
    return;
  }
  (posDelta > 0) ? _startUnfolding() : _startFolding();
}
