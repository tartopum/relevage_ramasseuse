#include "Actuator.h"

Actuator::Actuator(
  distance dMax,
  distance posAccuracy,
  int posInputMin,
  int posInputMax,
  byte posInputPin,
  byte isClosedInputPin,
  byte cmdOutputPin
) {
  _dMax = dMax;
  _posAccuracy = posAccuracy;
  _posInputMin = posInputMin;
  _posInputMax = posInputMax;
  _posInputPin = posInputPin;
  _isClosedInputPin = isClosedInputPin;
  _cmdOutputPin = cmdOutputPin;

  pinMode(_posInputPin, INPUT);
  pinMode(_isClosedInputPin, INPUT);
  pinMode(_cmdOutputPin, OUTPUT);
}

float Actuator::_readPosRatio() {
  const int inputVal = analogRead(_posInputPin);
  return float posRatio = (float)(inputVal - _posInputMin) / (float)(_posInputMax - _posInputMin);
}

distance _computeDistanceFromRatio(float ratio) {
  return _dMin + ratio * (_dMax - _dMin);
}

distance _readPos() {
  return _computeDistanceFromRatio(_readPosRatio());
}

void _isClosed() {

}

void _goLonger() {

}

void _goShorter() {

}

void Actuator::goTo(float targetRatio) {
  distance target = _computeDistanceFromRatio(targetRatio);
  while (true) {
    int delta = target - _readPos();
    if (abs(delta) < _posAccuracy || delta < 0 && _isClosed()) {
      break;
    }
    (delta > 0) ? _goLonger() : _goShorter();
  }
}
