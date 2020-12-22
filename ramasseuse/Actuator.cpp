#include "Actuator.h"

Actuator::Actuator(
  int posPerThousandAccuracy,
  float posInputMinVolts,
  float posInputMaxVolts,
  byte posInputPin,
  byte isTotallyFoldedInputPin,
  byte r1Pin,
  byte r2Pin,
  byte r3Pin,
  byte r4Pin
) : BaseActuator(posPerThousandAccuracy, posInputMinVolts, posInputMaxVolts, posInputPin)
{
  _isTotallyFoldedInputPin = isTotallyFoldedInputPin;
  _r1Pin = r1Pin;
  _r2Pin = r2Pin;
  _r3Pin = r3Pin;
  _r4Pin = r4Pin;

  pinMode(_isTotallyFoldedInputPin, INPUT);
  pinMode(_r1Pin, OUTPUT);
  pinMode(_r2Pin, OUTPUT);
  pinMode(_r3Pin, OUTPUT);
  pinMode(_r4Pin, OUTPUT);
}

bool Actuator::_isTotallyFolded() {
  return digitalRead(_isTotallyFoldedInputPin) == HIGH;
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
