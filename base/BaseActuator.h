#ifndef BaseActuator_h
#define BaseActuator_h

#include "Arduino.h"

class BaseActuator {
  public:
    BaseActuator(
      int posPerThousandAccuracy,
      float posInputMinVolts,
      float posInputMaxVolts,
      byte posInputPin
    );

    void startMovingTo(int targetPerThousand);
    void stop();
    bool isFolding();
    bool isUnfolding();

  protected:
    // Le pin pour lire la valeur du capteur de position du verin.
    byte _posInputPin;

    // On utilise des pour mille pour conserver des nombres entiers : 5.5% = 55pm
    int _posPerThousandAccuracy;
    // La valeur de analogRead() du capteur de position quand le verin est totalement ferme.
    int _posInputMin;
    // La valeur de analogRead() du capteur de position quand le verin est totalement ouvert.
    int _posInputMax;

    bool _moving = false;
    bool _folding = false;

    int _readPosPerThousand();
    int _computePosDelta();
    virtual void _startFolding() = 0;
    virtual void _startUnfolding() = 0;
    virtual void _stop() = 0;
    virtual bool _isTotallyFolded() = 0;
    virtual bool _isTotallyUnfolded() = 0;
};

#endif
