#ifndef BaseActuator_h
#define BaseActuator_h

#include "Arduino.h"

typedef enum {
  NO_STOP = 0,
  STOP_AT_POS = 1,
  STOP_FOLDED = 2,
  STOP_UNFOLDED = 3,
  STOP_BLOCKED = 4,
  STOP_TOO_SLOW = 5
} actuator_stop_reason_t;

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
      unsigned int minSpeedCheckPeriod
    );

    void startMovingTo(int target);
    int readLen();
    actuator_stop_reason_t stopIfNecessary();
    void stop();
    bool isFolding();
    bool isUnfolding();
    bool isTotallyFolded();
    bool isTotallyUnfolded();

  protected:
    // Le pin pour lire la valeur du capteur de position du verin.
    byte _lenInputPin;

    // On a un capteur a part, en plus du capteur de position, par mesure de
    // securite pour la fin de course.
    //
    // Si pas de capteur de fin de course, mettre a 0.
    byte _isTotallyFoldedInputPin = 0;
    byte _isTotallyUnfoldedInputPin = 0;

    // On utilise des pour mille pour conserver des nombres entiers.
    // Par exemple, une precision de 5mm sur une course de 1m correspondrait a
    // une precision de 5.5 si on travaillait en pourcentages. Avec des pour mille,
    // on a une precision definie a 5.
    int _lenAccuracy;

    int _targetLen = -1;
 
    // Les valeurs de analogRead() du capteur de position aux positions extremes
    int _foldedInputVal;
    int _unfoldedInputVal;

    bool _moving = false;
    bool _folding = false;

    // Pour detecter un dysfonctionnement du verin, on s'assure que sa vitesse
    // de deplacement n'est pas inferieure a une certaine valeur.
    int _lastMinSpeedCheckLen = -1;
    unsigned long _lastMinSpeedCheckTime = 0;
    int _minSpeedAlert = 0;  // En pour-mille/s
    unsigned int _minSpeedCheckPeriod = 1000;

    bool _isTooSlow();
    virtual bool _looksBlocked() = 0;
    virtual void _startFolding() = 0;
    virtual void _startUnfolding() = 0;
    virtual void _stop() = 0;
};

#endif
