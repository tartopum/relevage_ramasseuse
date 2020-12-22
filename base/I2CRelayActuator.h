#ifndef I2CRelayActuator_h
#define I2CRelayActuator_h

#include "Arduino.h"
#include "BaseActuator.h"
#include <multi_channel_relay.h>

class I2CRelayActuator: public BaseActuator {
  public:
    I2CRelayActuator(
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
    );

  protected:
    Multi_Channel_Relay _relay;

    bool _stopRelayStates[4];
    bool _foldingRelayStates[4];
    bool _unfoldingRelayStates[4];

    void _cmdRelays(bool states[4]);
    void _startFolding();
    void _startUnfolding();
    void _stop();
};

#endif
