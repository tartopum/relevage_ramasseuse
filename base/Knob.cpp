#include "Knob.h"

Knob::Knob(
  int foldedInputVal,
  int unfoldedInputVal,
  byte pin,
  int inputValMaxNoise,
  unsigned long readDelay
) {
  _foldedInputVal = foldedInputVal;
  _unfoldedInputVal = unfoldedInputVal;
  _inputValScale = abs(_unfoldedInputVal - _foldedInputVal);
  _pin = pin;
  _inputValMaxNoise = inputValMaxNoise;
  _readDelay = readDelay;

  pinMode(_pin, INPUT);
}

bool Knob::_isFoldedVal(int inputVal) {
  // La position extreme replie peut etre utilisee comme commande speciale (par
  // exemple pour desactiver une alarme), donc on s'assure qu'on retourne bien 0
  // quand le capteur est en position extreme (et non une valeur un peu superieure
  // due a du bruit).
  if (_foldedInputVal < _unfoldedInputVal) {
    return inputVal <= (_foldedInputVal + _inputValMaxNoise);
  }
  return inputVal >= (_foldedInputVal - _inputValMaxNoise);
}

bool Knob::_isUnfoldedVal(int inputVal) {
  // La position extreme deplie peut etre utilisee comme commande speciale, donc
  // on s'assure qu'on retourne bien 1000 quand le capteur est en position extreme
  // (et non une valeur un peu inferieure due a du bruit).
  if (_foldedInputVal < _unfoldedInputVal) {
    return inputVal >= (_unfoldedInputVal - _inputValMaxNoise);
  }
  return inputVal <= (_unfoldedInputVal + _inputValMaxNoise);
}

int Knob::readTargetLen() {
  const int inputVal = analogRead(_pin);
 
  // On initialise
  if (_prevInputVal == -1) {
    _prevInputVal = inputVal;
  }

  // Si la valeur n'a pas change en dehors du bruit, on retourne NO_TARGET_LEN_CHANGE
  // Quand la valeur change significativement, on enregistre le moment du dernier
  // changement et ne retourne la nouvelle valeur qu'au bout de _readDelay.
  // Ainsi, quand l'usager tourne le potentiometre on ne retourne pas une nouvelle
  // valeur toutes les ~100ms mais on attend qu'il ait arrete le potentiometre.
  // Cela permet d'eviter les multiples micro-deplacements de ce genre, quand
  // on tourne lentement le potentiometre de 50 a 60 :
  //
  // * Longueur = 50
  // * Longueur cible = 53
  // * Demarrage du verin
  // * Longueur = 53
  // * Arret du verin
  // * Longueur cible = 56
  // * Demarrage du verin
  // * Longueur = 56
  // * Arret du verin
  // * Longueur cible = 60
  // * Demarrage du verin
  // * Longueur = 60
  // * Arret du verin
  //
  // Avec le delai, on a :
  //
  // * Longueur = 50
  // * Longueur cible = 53
  // * Attente de _readDelay
  // * Longueur cible = 56
  // * Attente de _readDelay
  // * Longueur cible = 60
  // * Attente de _readDelay
  // * Demarrage du verin
  // * Longueur = 60
  // * Arret du verin
  int delta = abs(_prevInputVal - inputVal);
  bool isTurned = delta > _inputValMaxNoise;
  bool changedALongTimeAgo = (millis() - _lastChangeTime) >= _readDelay;

  if (isTurned) {
    _lastChangeTime = millis();
    _prevInputVal = inputVal;
    _valReturned = false;
    return NO_TARGET_LEN_CHANGE;
  } else if (!changedALongTimeAgo || _valReturned) {
    return NO_TARGET_LEN_CHANGE;
  }

  _valReturned = true;

  if (_isFoldedVal(inputVal)) {
    return 0;
  }
  if (_isUnfoldedVal(inputVal)) {
    return MAX_TARGET_LEN;
  }
  float deltaToFolded = abs(inputVal - _foldedInputVal);
  return deltaToFolded / _inputValScale * MAX_TARGET_LEN;
}
