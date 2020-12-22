#ifndef Actuator_h
#define Actuator_h

#include "Arduino.h"
#include "base/BaseActuator.h"

class Actuator: public BaseActuator {
  public:
    Actuator(
      int posPerThousandAccuracy,
      int posInputMin,
      int posInputMax,
      byte posInputPin,
      byte isTotallyFoldedInputPin,
      byte cmdOutputPin
    );

  protected:
    // Fin de course : verin totalement ferme
    // On a un capteur a part, en plus du capteur de position, par mesure de
    // securite.
    // A priori, la fin de course totalement ouvert ne sera jamais atteinte,
    // donc on ne definit pas d'entree correspondante.
    byte _isTotallyFoldedInputPin;
    byte _cmdOutputPin;

    // On utilise des pour mille pour conserver des nombres entiers : 5.5% = 55pm
    int _posPerThousandAccuracy;
    // La valeur du potar quand le verin est totalement ferme
    int _posInputMin;
    // La valeur du potar quand le verin est totalement ouvert
    int _posInputMax;

    void _stop();
    void _startFolding();
    void _startUnfolding();
    bool _isTotallyFolded();
    bool _isTotallyUnfolded();
};

#endif
