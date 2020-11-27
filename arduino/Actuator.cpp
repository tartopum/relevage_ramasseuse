#include "Actuator.h"

Actuator::Actuator(
  int posPerThousandAccuracy,
  int posInputMin,
  int posInputMax,
  byte posInputPin,
  byte isTotallyFoldedInputPin,
  byte cmdOutputPin
) {
  _posPerThousandAccuracy = posPerThousandAccuracy;
  _posInputMin = posInputMin;
  _posInputMax = posInputMax;
  _posInputPin = posInputPin;
  _isTotallyFoldedInputPin = isTotallyFoldedInputPin;
  _cmdOutputPin = cmdOutputPin;

  pinMode(_posInputPin, INPUT);
  pinMode(_isTotallyFoldedInputPin, INPUT);
  pinMode(_cmdOutputPin, OUTPUT);
}

int Actuator::_readPosPerThousand() {
  const int inputVal = analogRead(_posInputPin);
  const float posRatio = (float)(inputVal - _posInputMin) / (float)(_posInputMax - _posInputMin);
  return (int)(posRatio * 1000);
}

bool Actuator::_isTotallyFolded() {

}

bool Actuator::_isTotallyUnfolded() {
  // On n'a pas de capteur pour ce cas, qui n'est pas cense arriver. Mais on
  // definit quand meme la methode pour la carte du code.
  return false;
}

void Actuator::_stepLonger() {

}

void Actuator::_stepShorter() {

}

void Actuator::stepTo(int targetPerThousand) {
  int posDelta = targetPerThousand - _readPosPerThousand();
  bool isAtPos = abs(posDelta) < _posPerThousandAccuracy;
  bool cannotStep = (delta < 0 && _isTotallyFolded()) || (delta > 0 && _isTotallyUnfolded());
  if (isAtPos || cannotStep) {
    return;
  }
  (delta > 0) ? _stepLonger() : _stepShorter();
}
