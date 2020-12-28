#include "BasePinRelayActuator.h"

BasePinRelayActuator::BasePinRelayActuator(
  int lenAccuracy,
  int foldedInputVal,
  int unfoldedInputVal,
  byte lenInputPin,
  byte isTotallyFoldedInputPin,
  byte isTotallyUnfoldedInputPin,
  int minSpeedAlert,
  unsigned int minSpeedCheckPeriod,
  byte relaySourceFoldPin,
  byte relaySourceUnfoldPin,
  byte relayMotorPin1,
  byte relayMotorPin2
) : BaseActuator(
  lenAccuracy,
  foldedInputVal,
  unfoldedInputVal,
  lenInputPin,
  isTotallyFoldedInputPin,
  isTotallyUnfoldedInputPin,
  minSpeedAlert,
  minSpeedCheckPeriod
)
{
  _relaySourceFoldPin = relaySourceFoldPin;
  _relaySourceUnfoldPin = relaySourceUnfoldPin;
  _relayMotorPin1 = relayMotorPin1;
  _relayMotorPin2 = relayMotorPin2;

  pinMode(_relaySourceFoldPin, OUTPUT);
  pinMode(_relaySourceUnfoldPin, OUTPUT);
  pinMode(_relayMotorPin1, OUTPUT);
  pinMode(_relayMotorPin2, OUTPUT);
}

void BasePinRelayActuator::_startFolding() {
  _setSourceRelaysForFolding();
  _connectMotorRelays();
}

void BasePinRelayActuator::_startUnfolding() {
  _setSourceRelaysForUnfolding();
  _connectMotorRelays();
}

void BasePinRelayActuator::_stop() {
  _disconnectMotorRelays();
}
