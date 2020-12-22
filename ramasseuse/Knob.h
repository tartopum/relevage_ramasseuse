#ifndef Knob_h
#define Knob_h

#include "Arduino.h"
#include "base/TargetPosReader.h"

class Knob: public TargetPosReader {
  public:
    Knob(
      int minInputVal,
      int maxInputVal,
      byte pin
    );

    float readTargetPosRatio();

  private:
    byte _pin;
    int _minInputVal;
    int _maxInputVal;
};

#endif
