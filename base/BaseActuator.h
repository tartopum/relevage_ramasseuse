#ifndef BaseActuator_h
#define BaseActuator_h

#include "Arduino.h"

class BaseActuator {
  public:
    BaseActuator(
      int posPerThousandAccuracy,
      int posInputMin,
      int posInputMax,
      byte posInputPin
    );

    void stepTo(int targetPerThousand);

  protected:
    byte _posInputPin;

    // On utilise des pour mille pour conserver des nombres entiers : 5.5% = 55pm
    int _posPerThousandAccuracy;
    // La valeur du potar quand le verin est totalement ferme
    int _posInputMin;
    // La valeur du potar quand le verin est totalement ouvert
    int _posInputMax;

    int _readPosPerThousand();
    int _computePosDelta();
    virtual void _stop() = 0;
    virtual void _startFolding() = 0;
    virtual void _startUnfolding() = 0;
    virtual bool _isTotallyFolded() = 0;
    virtual bool _isTotallyUnfolded() = 0;
};

#endif
