#include "RelayActuator.h"

RelayActuator::RelayActuator(
  int posPerThousandAccuracy,
  float posInputMinVolts,
  float posInputMaxVolts,
  byte posInputPin,
  byte isTotallyFoldedInputPin,
  byte isTotallyUnfoldedInputPin,
  unsigned long maxMovingTime,
  float movingTimeAlertCoef,
  byte relaySourceFoldPin,
  byte relaySourceUnfoldPin,
  byte relayMotorPin1,
  byte relayMotorPin2
) : BaseActuator(
  posPerThousandAccuracy,
  posInputMinVolts,
  posInputMaxVolts,
  posInputPin,
  isTotallyFoldedInputPin,
  isTotallyUnfoldedInputPin,
  maxMovingTime,
  movingTimeAlertCoef
)
{
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

void RelayActuator::_startFolding() {
  _setSourceRelaysForFolding();
  _connectMotorRelays();
}

void RelayActuator::_startUnfolding() {
  _setSourceRelaysForUnfolding();
  _connectMotorRelays();
}

void RelayActuator::_stop() {
  _disconnectMotorRelays();
}
