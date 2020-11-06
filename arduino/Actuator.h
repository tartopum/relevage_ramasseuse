#ifndef Actuator_h
#define Actuator_h

#include "Arduino.h"
#include "Types.h"

class Actuator {
  public:
    Actuator(
      distance dMax,
      distance posAccuracy,
      int posInputMin,
      int posInputMax,
      byte posInputPin,
      byte isClosedInputPin,
      byte cmdOutputPin
    );

    void goTo(float targetRatio);

  private:
    byte _posInputPin;
    byte _isClosedInputPin; // Fin de course totalement replie
    byte _cmdOutputPin;

    distance _dMin = 0;
    distance _dMax;
    distance _posAccuracy;
    int _posInputMin; // La valeur du potar quand le verin est totalement replie
    int _posInputMax; // La valeur du potar quand le verin est totalement deplie

    float _readPosRatio();
    distance _readPos();
    distance _computeDistanceFromRatio();
    void _goLonger();
    void _goShorter();
    void _isClosed();
};

#endif
