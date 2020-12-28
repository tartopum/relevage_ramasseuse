#ifndef I2CRelayActuator_h
#define I2CRelayActuator_h

#include "Arduino.h"
#include "BaseActuator.h"
#include <multi_channel_relay.h>

class I2CRelayActuator: public BaseActuator {
  public:
    I2CRelayActuator(
      int posAccuracy,
      int foldedInputVal,
      int unfoldedInputVal,
      byte posInputPin,
      byte isTotallyFoldedInputPin,
      byte isTotallyUnfoldedInputPin,
      int relayI2CAddr,
      uint8_t stopRelayState,
      uint8_t foldingRelayState,
      uint8_t unfoldingRelayState
    );

  protected:
    Multi_Channel_Relay _relay;

    uint8_t _stopRelayState;
    uint8_t _foldingRelayState;
    uint8_t _unfoldingRelayState;

    void _startFolding();
    void _startUnfolding();
    void _stop();
};

#endif
