#include "BaseActuator.h"

BaseActuator::BaseActuator(
  int posPerThousandAccuracy,
  float posInputMinVolts,
  float posInputMaxVolts,
  byte posInputPin,
  byte isTotallyFoldedInputPin,
  byte isTotallyUnfoldedInputPin,
  unsigned long maxMovingTime,
  float movingTimeAlertCoef
) {
    _posPerThousandAccuracy = posPerThousandAccuracy;
  _posInputMin = posInputMinVolts / 5 * 1023;
  _posInputMax = posInputMaxVolts / 5 * 1023;
  _posInputPin = posInputPin;

  _maxMovingTime = maxMovingTime;
  _movingTimeAlertCoef = movingTimeAlertCoef;

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
  if (!_expectedMovingTime) {
    // On n'a pas encore defini la date de debut de deplacement, il n'y en a pas
    // encore eu.
    return false;
  }
  return (millis() - _lastTargetChangeTime) > (_movingTimeAlertCoef * _expectedMovingTime);
}

void BaseActuator::startMovingTo(int targetPerThousand) {
  // Avant tout, on verifie si le verin est a la position cible. Le cas echeant,
  // on l'arrete.
  int posDelta = targetPerThousand - _readPosPerThousand();
  bool isAtPos = abs(posDelta) < _posPerThousandAccuracy;
  bool cannotStep = (posDelta < 0 && isTotallyFolded()) || (posDelta > 0 && isTotallyUnfolded());
  if (isAtPos || cannotStep) {
    stop();
    return;
  }

  // Sinon, on ne commande le verin que si la position cible a change.
  // Cela evite d'envoyer des signaux a repetition a l'electrovanne.
  int deltaWithLastTarget = abs(targetPerThousand - _lastTargetPerThousand);
  if (_lastTargetPerThousand != -1 && deltaWithLastTarget < _posPerThousandAccuracy) {
    return;
  }

  // Si la position cible a change, on enregistre l'heure courante pour verifier
  // que le verin n'est pas bloque (s'il met trop de temps a atteindre la cible).
  //
  // Comme on a verifie au prealable que la position cible avait bien change,
  // on ne met bien a jour ces variables qu'au debut d'un deplacement.
  _lastTargetChangeTime = millis();
  _lastTargetPerThousand = targetPerThousand;
  _expectedMovingTime = (float)(deltaWithLastTarget) / 1000 * _maxMovingTime;

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
