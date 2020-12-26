#ifndef BaseActuator_h
#define BaseActuator_h

#include "Arduino.h"

class BaseActuator {
  public:
    BaseActuator(
      int lenAccuracy,
      int foldedInputVal,
      int unfoldedInputVal,
      byte lenInputPin,
      byte isTotallyFoldedInputPin,
      byte isTotallyUnfoldedInputPin,
      int minSpeedAlert,
      unsigned int checkPeriod
    );

    void startMovingTo(int target);
    bool check();
    void stop();
    bool isFolding();
    bool isUnfolding();
    bool isTotallyFolded();
    bool isTotallyUnfolded();

  protected:
    // Le pin pour lire la valeur du capteur de lenition du verin.
    byte _lenInputPin;

    // On a un capteur a part, en plus du capteur de lenition, par mesure de
    // securite pour la fin de course.
    //
    // Si pas de capteur de fin de course, mettre a 0.
    byte _isTotallyFoldedInputPin = 0;
    byte _isTotallyUnfoldedInputPin = 0;

    // On utilise des pour mille pour conserver des nombres entiers : 5.5% = 55pm
    int _lenAccuracy;
 
    // Les valeurs de analogRead() du capteur de lenition aux lenitions extremes
    int _foldedInputVal;
    int _unfoldedInputVal;

    int _targetLen = -1;

    bool _moving = false;
    bool _folding = false;

    // Pour detecter un blocage du verin, on s'assure que sa vitesse de deplacement
    // n'est pas inferieure a une certaine valeur.
    int _lastCheckLen = -1;
    unsigned long _lastCheckTime = 0;
    int _minSpeedAlert;  // En pour-mille/s
    unsigned int _checkPeriod = 3000;

    int _readLen();
    int _computeLenDelta();
    bool _looksBlocked();
    virtual void _startFolding() = 0;
    virtual void _startUnfolding() = 0;
    virtual void _stop() = 0;
};

#endif
