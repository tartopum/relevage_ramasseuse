#include "BaseActuator.h"

BaseActuator::BaseActuator(
  int lenAccuracy,
  int foldedInputVal,
  int unfoldedInputVal,
  byte lenInputPin,
  byte isTotallyFoldedInputPin,
  byte isTotallyUnfoldedInputPin,
  int minSpeedAlert,
  unsigned int minSpeedCheckPeriod
) {
  _lenAccuracy = lenAccuracy;
  _foldedInputVal = foldedInputVal;
  _unfoldedInputVal = unfoldedInputVal;
  _lenInputPin = lenInputPin;

  _isTotallyFoldedInputPin = isTotallyFoldedInputPin;
  _isTotallyUnfoldedInputPin = isTotallyUnfoldedInputPin;

  _minSpeedAlert = minSpeedAlert;
  _minSpeedCheckPeriod = minSpeedCheckPeriod;

  pinMode(_lenInputPin, INPUT);
  pinMode(_isTotallyFoldedInputPin, INPUT);
  pinMode(_isTotallyUnfoldedInputPin, INPUT);
}

int BaseActuator::readLen() {
  /*
   * Selon les branchements du potentiometre, les valeurs basses du capteur
   * correspondent a une position repliee ou depliee du verin.
   */
  const int inputVal = analogRead(_lenInputPin);
 
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

bool BaseActuator::_isTooSlow() {
  int len = readLen();
  unsigned long now = millis();
  unsigned long duration = now - _lastMinSpeedCheckTime;

  if (!_moving) {
    // On met a jour la date de verification, sinon au moment de relancer un
    // deplacement, la periode de verification sera passee et la vitesse sera
    // calculee sans moyenne.
    _lastMinSpeedCheckTime = now;
    _lastMinSpeedCheckLen = len;
    return false;
  }
  if (_lastMinSpeedCheckLen == -1) {
    _lastMinSpeedCheckTime = now;
    _lastMinSpeedCheckLen = len;
    return false;
  }

  // On attend que le deplacement ait dure un certain temps avant de calculer
  // la vitesse, sinon on est trop soumis aux petits aleas de deplacement.
  if (duration < _minSpeedCheckPeriod) {
    return false;
  }

  float movingTime = (float)duration / 1000;
  int speed = abs(len - _lastMinSpeedCheckLen) / movingTime;

  _lastMinSpeedCheckLen = len;
  _lastMinSpeedCheckTime = now;

  return speed <= _minSpeedAlert;
};

actuator_stop_reason_t BaseActuator::stopIfNecessary() {
  if (_looksBlocked()) {
    stop();
    return STOP_BLOCKED;
  }

  if (_isTooSlow()) {
    stop();
    return STOP_TOO_SLOW;
  }

  int lenDelta = _targetLen - readLen();
  if (abs(lenDelta) < _lenAccuracy) {
    stop();
    return STOP_AT_POS;
  }
  if (lenDelta < 0 && isTotallyFolded()) {
    stop();
    return STOP_FOLDED;
  }
  if (lenDelta > 0 && isTotallyUnfolded()) {
    stop();
    return STOP_UNFOLDED;
  }

  return NO_STOP;
}

void BaseActuator::startMovingTo(int target) {
  _targetLen = target;

  // On ne demarre pas si on va s'arreter tout de suite apres.
  actuator_stop_reason_t stopReason = stopIfNecessary();
  if (stopReason != NO_STOP) {
    return;
  }

  int lenDelta = _targetLen - readLen();

  if (lenDelta > 0 && !isUnfolding()) {
    _startUnfolding();
    _folding = false;
  } else if (lenDelta < 0 && !isFolding()) {
    _startFolding();
    _folding = true;
  }  

  _moving = true;
}

void BaseActuator::stop() {
  _moving = false;
  _stop();
}
