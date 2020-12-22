#include "Actuator.h"

Actuator::Actuator(
  int posPerThousandAccuracy,
  int posInputMin,
  int posInputMax,
  byte posInputPin,
  byte isTotallyFoldedInputPin,
  byte cmdOutputPin
) : BaseActuator(posPerThousandAccuracy, posInputMin, posInputMax, posInputPin)
{
  _isTotallyFoldedInputPin = isTotallyFoldedInputPin;
  _cmdOutputPin = cmdOutputPin;

  pinMode(_isTotallyFoldedInputPin, INPUT);
  pinMode(_cmdOutputPin, OUTPUT);
}

bool Actuator::_isTotallyFolded() {
  return false; // TODO
}

bool Actuator::_isTotallyUnfolded() {
  // On n'a pas de capteur pour ce cas, qui n'est pas cense arriver. Mais on
  // definit quand meme la methode pour la carte du code.
  return false;
}

void Actuator::_startFolding() {

}

void Actuator::_startUnfolding() {

}

void Actuator::_stop() {

}
