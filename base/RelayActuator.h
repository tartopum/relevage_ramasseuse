#ifndef Actuator_h
#define Actuator_h

#include "Arduino.h"
#include "BaseActuator.h"

class RelayActuator: public BaseActuator {
  public:
    RelayActuator(
      int posPerThousandAccuracy,
      float posInputMinVolts,
      float posInputMaxVolts,
      byte posInputPin,
      byte isTotallyFoldedInputPin,
      byte relaySourceFoldPin,
      byte relaySourceUnfoldPin,
      byte relayMotorPin1,
      byte relayMotorPin2
    );

    void stop();

  protected:
    // Fin de course : verin totalement ferme
    // On a un capteur a part, en plus du capteur de position, par mesure de
    // securite.
    // A priori, la fin de course totalement ouvert ne sera jamais atteinte,
    // donc on ne definit pas d'entree correspondante.
    byte _isTotallyFoldedInputPin;

    // Envoient du (+) ou du (-) sur les pins de l'electrovanne.
    //
    // relaySourceFold est connecte au pin de l'electrovanne qui a besoin de (+)
    // pour que le verin se replie.
    byte _relaySourceFoldPin;
    // relaySourceUnfold est connecte au pin de l'electrovanne qui a besoin de (+)
    // pour que le verin se deplie.
    byte _relaySourceUnfoldPin;

    // Transmettent le courant des relais sources a l'electrovanne ou coupent le courant.
    byte _relayMotorPin1;
    byte _relayMotorPin2;

    virtual void _setFoldSourceRelays() = 0;
    virtual void _setUnfoldSourceRelays() = 0;
    virtual void _connectMotorRelays() = 0;
    virtual void _disconnectMotorRelays() = 0;
    void _startFolding();
    void _startUnfolding();
    bool _isTotallyFolded();
    bool _isTotallyUnfolded();
};

#endif
