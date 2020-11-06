#ifndef Knob_h
#define Knob_h

#include "Arduino.h"
#include "Types.h"

class Knob {
  public:
    Knob(
      int minInputVal,
      int maxInputVal,
      byte pin
    );

    // Le potar de cabine ne retourne que des positions sous forme de ratio.
    // Comme ca, la conversion ratio -> distance ne se fait qu'au niveau
    // de la classe Actuator().
    float readTargetPosRatio();

  private:
    byte _pin;
    int _minInputVal;
    int _maxInputVal;
};

#endif
