#ifndef BaseActuator_h
#define BaseActuator_h

#include "Arduino.h"

class BaseActuator {
  public:
    BaseActuator(
      int posPerThousandAccuracy,
      float posInputMinVolts,
      float posInputMaxVolts,
      byte posInputPin,
      unsigned long maxMovingTime,
      float movingTimeAlertCoef
    );

    void startMovingTo(int targetPerThousand);
    void stop();
    bool isFolding();
    bool isUnfolding();
    bool looksBlocked();

  protected:
    // Le pin pour lire la valeur du capteur de position du verin.
    byte _posInputPin;

    // On utilise des pour mille pour conserver des nombres entiers : 5.5% = 55pm
    int _posPerThousandAccuracy;
    // La valeur de analogRead() du capteur de position quand le verin est totalement ferme.
    int _posInputMin;
    // La valeur de analogRead() du capteur de position quand le verin est totalement ouvert.
    int _posInputMax;

    bool _moving = false;
    bool _folding = false;

    // Pour detecter un blocage du verin, on s'assure qu'il n'est pas en
    // deplacement depuis plus d'un certain temps depuis la derniere
    // fois qu'on a change de position cible (si on change cette derniere
    // toutes les 2 secondes, il est normal que le verin soit en permanence
    // en mouvement).
    int _lastTargetPerThousand = -1;
    unsigned long _lastTargetChangeTime = 0;
    unsigned long _maxMovingTime;
    unsigned long _expectedMovingTime = 0;
    float _movingTimeAlertCoef;

    int _readPosPerThousand();
    int _computePosDelta();
    virtual void _startFolding() = 0;
    virtual void _startUnfolding() = 0;
    virtual void _stop() = 0;
    virtual bool _isTotallyFolded() = 0;
    virtual bool _isTotallyUnfolded() = 0;
};

#endif
