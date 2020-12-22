#ifndef Actuator_h
#define Actuator_h

#include "Arduino.h"
#include "BaseActuator.h"

class RelayActuator: public BaseActuator {
  public:
    RelayActuator(
      int posPerThousandAccuracy,
      float posInputMinVolts,
      float posInputMaxVolts,
      byte posInputPin,
      unsigned long maxMovingTime,
      byte isTotallyFoldedInputPin,
      byte relaySourceFoldPin,
      byte relaySourceUnfoldPin,
      byte relayMotorPin1,
      byte relayMotorPin2
    );

  protected:
    // Fin de course : verin totalement ferme
    // On a un capteur a part, en plus du capteur de position, par mesure de
    // securite.
    // A priori, la fin de course totalement ouvert ne sera jamais atteinte,
    // donc on ne definit pas d'entree correspondante.
    byte _isTotallyFoldedInputPin;

    // Les relais sources permettent de decider si c'est du (+) ou du (-) qui est
    // envoye sur chaque pin de l'electrovanne. Selon la tension que recoit chaque
    // pin de l'electrovanne, le verin se deplie ou se replie. Inverser la polarite
    // inverse le sens de deplacement.
    //
    // Pour REPLIER le verin :
    // * Commander relaySourceFold pour le connecter au (+)
    // * Commander relaySourceUnfold pour le connecter au (-)
    //
    // Pour DEPLIER le verin :
    // * Commander relaySourceFold pour le connecter au (-)
    // * Commander relaySourceUnfold pour le connecter au (+)
    //
    // Les commandes a envoyer aux relais dependent de leur position par defaut,
    // de s'ils sont connectes par defaut au (+) ou au (-).
    byte _relaySourceFoldPin;
    byte _relaySourceUnfoldPin;

    // Les relais moteur transmettent le courant des relais sources a l'electrovanne
    // ou coupent cette derniere du courant. Ils assurent la fonction ON/OFF.
    //
    // Pour que le verin se deplace, il faut que les deux pins de l'electrovanne
    // soient connectes aux sources de courant.
    //
    // Pour arreter le verin, il faut deconnecter les deux pins de l'electrovanne.
    //
    // Ce que ca implique sur la commande des relais depend de s'ils sont
    // normalement ouverts ou normalement fermes.
    byte _relayMotorPin1;
    byte _relayMotorPin2;

    // Connecte relaySourceFold au (+) et relaySourceUnfold au (-)
    virtual void _setSourceRelaysForFolding() = 0;

    // Connecte relaySourceFold au (-) et relaySourceUnfold au (+)
    virtual void _setSourceRelaysForUnfolding() = 0;

    // Ferme les relais moteur pour que le courant atteigne l'electrovanne
    virtual void _connectMotorRelays() = 0;

    // Ouvre les relais moteur pour que le courant n'atteigne pas l'electrovanne
    virtual void _disconnectMotorRelays() = 0;

    void _startFolding();
    void _startUnfolding();
    void _stop();
    bool _isTotallyFolded();
    bool _isTotallyUnfolded();
};

#endif
