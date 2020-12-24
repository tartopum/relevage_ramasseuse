#include "I2CRelayActuator.h"

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
  uint8_t stopRelayState,
  uint8_t foldingRelayState,
  uint8_t unfoldingRelayState
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

  _stopRelayState = stopRelayState;
  _foldingRelayState = foldingRelayState;
  _unfoldingRelayState = unfoldingRelayState;
}

void I2CRelayActuator::_startFolding() {
  _relay.channelCtrl(_foldingRelayState);
}

void I2CRelayActuator::_startUnfolding() {
  _relay.channelCtrl(_unfoldingRelayState);
}

void I2CRelayActuator::_stop() {
  _relay.channelCtrl(_stopRelayState);
}