#ifndef Knob_h
#define Knob_h

#include "Arduino.h"
#include "TargetLenReader.h"

const int NO_TARGET_LEN_CHANGE = -1;
const int MAX_TARGET_LEN = 1000;

class Knob: public TargetLenReader {
  public:
    Knob(
      int foldedInputVal,
      int unfoldedInputVal,
      byte pin,
      int noise
    );

    int readTargetLen();

  private:
    byte _pin;
    int _foldedInputVal;
    int _unfoldedInputVal;
    int _prevInputVal = -1;
    int _noise;
    float _inputValScale;

    bool _isFoldedVal(int inputVal);
    bool _isUnfoldedVal(int inputVal);
};

#endif
