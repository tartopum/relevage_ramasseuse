#ifndef Knob_h
#define Knob_h

#include "Arduino.h"
#include "TargetLenReader.h"

class Knob: public TargetLenReader {
  public:
    Knob(
      int foldedInputVal,
      int unfoldedInputVal,
      byte pin
    );

    int readTargetLen();

  private:
    byte _pin;
    int _foldedInputVal;
    int _unfoldedInputVal;
};

#endif
