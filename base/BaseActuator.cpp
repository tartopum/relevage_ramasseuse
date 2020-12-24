#include "BaseActuator.h"

BaseActuator::BaseActuator(
  int posAccuracy,
  float posInputMinVolts,
  float posInputMaxVolts,
  byte posInputPin,
  byte isTotallyFoldedInputPin,
  byte isTotallyUnfoldedInputPin,
  int minSpeedAlert,
  unsigned int checkPeriod
) {
    _posAccuracy = posAccuracy;
  _posInputMin = posInputMinVolts / 5 * 1023;
  _posInputMax = posInputMaxVolts / 5 * 1023;
  _posInputPin = posInputPin;

  _minSpeedAlert = minSpeedAlert;
  _checkPeriod = checkPeriod;

  _isTotallyFoldedInputPin = isTotallyFoldedInputPin;
  _isTotallyUnfoldedInputPin = isTotallyUnfoldedInputPin;

  pinMode(_posInputPin, INPUT);
  pinMode(_isTotallyFoldedInputPin, INPUT);
  pinMode(_isTotallyUnfoldedInputPin, INPUT);
}

int BaseActuator::_readPos() {
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
  if (_lastCheckPos == -1) {
    _lastCheckPos = _readPos();
    _lastCheckTime = millis();
    return false;
  }

  int pos = _readPos();
  unsigned long now = millis();
  unsigned long duration = now - _lastCheckTime;

  // On attend que le deplacement ait dure un certain temps avant de calculer
  // la vitesse, sinon on est trop soumis aux petits aleas de deplacement.
  if (duration < _checkPeriod) {
    return false;
  }

  float movingTime = (float)duration / 1000;
  int speed = abs(pos - _lastCheckPos) / movingTime;
  
  _lastCheckPos = pos;
  _lastCheckTime = now;

  return speed > _minSpeedAlert;
}

void BaseActuator::startMovingTo(int target) {
  int posDelta = target - _readPos();
  bool isAtPos = abs(posDelta) < _posAccuracy;
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
