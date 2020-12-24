#include "BaseActuator.h"

BaseActuator::BaseActuator(
  int posPerThousandAccuracy,
  float posInputMinVolts,
  float posInputMaxVolts,
  byte posInputPin,
  byte isTotallyFoldedInputPin,
  byte isTotallyUnfoldedInputPin,
  int minSpeedAlert
) {
    _posPerThousandAccuracy = posPerThousandAccuracy;
  _posInputMin = posInputMinVolts / 5 * 1023;
  _posInputMax = posInputMaxVolts / 5 * 1023;
  _posInputPin = posInputPin;

  _minSpeedAlert = minSpeedAlert;

  _isTotallyFoldedInputPin = isTotallyFoldedInputPin;
  _isTotallyUnfoldedInputPin = isTotallyUnfoldedInputPin;

  pinMode(_posInputPin, INPUT);
  pinMode(_isTotallyFoldedInputPin, INPUT);
  pinMode(_isTotallyUnfoldedInputPin, INPUT);
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

bool BaseActuator::isTotallyFolded() {
  if (!_isTotallyFoldedInputPin) {
    return false;
  }
  return digitalRead(_isTotallyFoldedInputPin) == HIGH;
}

bool BaseActuator::isTotallyUnfolded() {
  if (!_isTotallyUnfoldedInputPin) {
    return false;
  }
  return digitalRead(_isTotallyUnfoldedInputPin) == HIGH;
}

bool BaseActuator::looksBlocked() {
  if (!_moving) {
    return false;
  }
  if (_lastCheckPosPerThousand == -1) {
    _lastCheckPosPerThousand = _readPosPerThousand();
    _lastCheckTime = millis();
    return false;
  }
  int pos = _readPosPerThousand();
  unsigned long now = millis();
  float movingTime = (float)(now - _lastCheckTime) / 1000;
  int speed = abs(pos - _lastCheckPosPerThousand) / movingTime;
  
  _lastCheckPosPerThousand = pos;
  _lastCheckTime = now;

  return speed > _minSpeedAlert;
}

void BaseActuator::startMovingTo(int targetPerThousand) {
  int posDelta = targetPerThousand - _readPosPerThousand();
  bool isAtPos = abs(posDelta) < _posPerThousandAccuracy;
  bool cannotStep = (posDelta < 0 && isTotallyFolded()) || (posDelta > 0 && isTotallyUnfolded());
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
