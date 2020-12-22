#ifndef Actuator_h
#define Actuator_h

#include "Arduino.h"
#include "src/BaseActuator.h"

class Actuator: public BaseActuator {
  public:
    Actuator(
      int posPerThousandAccuracy,
      float posInputMinVolts,
      float posInputMaxVolts,
      byte posInputPin,
      byte isTotallyFoldedInputPin,
      byte r1Pin,
      byte r2Pin,
      byte r3Pin,
      byte r4Pin
    );

  protected:
    // Fin de course : verin totalement ferme
    // On a un capteur a part, en plus du capteur de position, par mesure de
    // securite.
    // A priori, la fin de course totalement ouvert ne sera jamais atteinte,
    // donc on ne definit pas d'entree correspondante.
    byte _isTotallyFoldedInputPin;
    byte _r1Pin;
    byte _r2Pin;
    byte _r3Pin;
    byte _r4Pin;

    void _stop();
    void _startFolding();
    void _startUnfolding();
    bool _isTotallyFolded();
    bool _isTotallyUnfolded();
};

#endif
