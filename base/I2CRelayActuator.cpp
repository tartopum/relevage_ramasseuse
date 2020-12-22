#include "I2CRelayActuator.h"

void copyArray(bool* src, bool *dest, int size) {
  for (int i = 0; i < size; i++) {
    dest[i] = src[i];
  }
}

I2CRelayActuator::I2CRelayActuator(
  int posPerThousandAccuracy,
  float posInputMinVolts,
  float posInputMaxVolts,
  byte posInputPin,
  byte isTotallyFoldedInputPin,
  byte isTotallyUnfoldedInputPin,
  unsigned long maxMovingTime,
  float movingTimeAlertCoef,
  int relayI2CAddr,
  bool stopRelayStates[4],
  bool foldingRelayStates[4],
  bool unfoldingRelayStates[4]
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
  _relay.begin(relayI2CAddr);

  copyArray(stopRelayStates, _stopRelayStates, 4);
  copyArray(foldingRelayStates, _foldingRelayStates, 4);
  copyArray(unfoldingRelayStates, _unfoldingRelayStates, 4);
}

void I2CRelayActuator::_cmdRelays(bool states[4]) {
  for (byte i = 0; i < 5; i++) {
    states[i] ? _relay.turn_on_channel(i) : _relay.turn_off_channel(i);
  }
}

void I2CRelayActuator::_startFolding() {
  _cmdRelays(_foldingRelayStates);
}

void I2CRelayActuator::_startUnfolding() {
  _cmdRelays(_unfoldingRelayStates);
}

void I2CRelayActuator::_stop() {
  _cmdRelays(_stopRelayStates);
}
