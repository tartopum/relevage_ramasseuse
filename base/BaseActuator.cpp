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

bool BaseActuator::isFolding() {
  return _moving && _folding;
}

bool BaseActuator::isUnfolding() {
  return _moving && !_folding;
}

void BaseActuator::startMovingTo(int targetPerThousand) {
  int posDelta = targetPerThousand - _readPosPerThousand();
  bool isAtPos = abs(posDelta) < _posPerThousandAccuracy;
  bool cannotStep = (posDelta < 0 && _isTotallyFolded()) || (posDelta > 0 && _isTotallyUnfolded());
  if (isAtPos || cannotStep) {
    stop();
    return;
  }

  _moving = true;
  if (posDelta > 0) {
    _startUnfolding();
    _folding = false;
  } else {
    _startFolding();
    _folding = true;
  }  
}

void BaseActuator::stop() {
  _moving = false;
  _stop();
}
