#include "RelayActuator.h"

RelayActuator::RelayActuator(
  int posPerThousandAccuracy,
  float posInputMinVolts,
  float posInputMaxVolts,
  byte posInputPin,
  byte isTotallyFoldedInputPin,
  byte relaySourceFoldPin,
  byte relaySourceUnfoldPin,
  byte relayMotorPin1,
  byte relayMotorPin2
) : BaseActuator(posPerThousandAccuracy, posInputMinVolts, posInputMaxVolts, posInputPin)
{
  _isTotallyFoldedInputPin = isTotallyFoldedInputPin;
  _relaySourceFoldPin = relaySourceFoldPin;
  _relaySourceUnfoldPin = relaySourceUnfoldPin;
  _relayMotorPin1 = relayMotorPin1;
  _relayMotorPin2 = relayMotorPin2;

  pinMode(_isTotallyFoldedInputPin, INPUT);
  pinMode(_relaySourceFoldPin, OUTPUT);
  pinMode(_relaySourceUnfoldPin, OUTPUT);
  pinMode(_relayMotorPin1, OUTPUT);
  pinMode(_relayMotorPin2, OUTPUT);
}

bool RelayActuator::_isTotallyFolded() {
  return digitalRead(_isTotallyFoldedInputPin) == HIGH;
}

bool RelayActuator::_isTotallyUnfolded() {
  // On n'a pas de capteur pour ce cas, qui n'est pas cense arriver. Mais on
  // definit quand meme la methode pour la carte du code.
  return false;
}

void RelayActuator::_startFolding() {
  _setFoldSourceRelays();
  _connectMotorRelays();
}

void RelayActuator::_startUnfolding() {
  _setUnfoldSourceRelays();
  _connectMotorRelays();
}

void RelayActuator::stop() {
  _disconnectMotorRelays();
}
