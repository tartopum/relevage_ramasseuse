#ifndef Knob_h
#define Knob_h

#include "Arduino.h"
#include "TargetPosReader.h"

class Knob: public TargetPosReader {
  public:
    Knob(
      float minInputVolts,
      float maxInputVolts,
      byte pin
    );

    int readTargetPosPerThousand();

  private:
    byte _pin;
    int _minInputVal;
    int _maxInputVal;
};

#endif
