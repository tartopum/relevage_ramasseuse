#ifndef Knob_h
#define Knob_h

#include "Arduino.h"
#include "src/TargetPosReader.h"

class Knob: public TargetPosReader {
  public:
    Knob(
      float minInputVolts,
      float maxInputVolts,
      byte pin
    );

    float readTargetPosRatio();

  private:
    byte _pin;
    int _minInputVal;
    int _maxInputVal;
};

#endif
