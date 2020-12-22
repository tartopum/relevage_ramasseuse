#include "src/RelayActuator.h"
#include "src/Knob.h"

/*
 * Variables
 */

// Potar
#define POTAR_MIN_VOLTS 0
#define POTAR_MAX_VOLTS 5
#define POTAR_PIN_ENTREE 0

// Precision
// Pour eviter les micro-deplacements permanents dus a des micro-variations des
// capteurs, ce qui abimerait les verins, on definit une marge d'erreur acceptable.
// On definit cette valeur en pour mille pour avoir des nombres entiers.
// 5,5% = 55 pour mille
#define PRECISION_POSITION_POUR_MILLE 50

// Verin gauche
#define VERIN_G_MIN_VOLTS 0
#define VERIN_G_MAX_VOLTS 5
#define VERIN_G_PIN_ENTREE 0
#define VERIN_G_PIN_FIN_COURSE 0
#define VERIN_G_PIN_R1 0
#define VERIN_G_PIN_R2 0
#define VERIN_G_PIN_R3 0
#define VERIN_G_PIN_R4 0

// Verin droit
#define VERIN_D_MIN_VOLTS 0
#define VERIN_D_MAX_VOLTS 5
#define VERIN_D_PIN_ENTREE 0
#define VERIN_D_PIN_FIN_COURSE 0
#define VERIN_D_PIN_R1 0
#define VERIN_D_PIN_R2 0
#define VERIN_D_PIN_R3 0
#define VERIN_D_PIN_R4 0


class Actuator : public RelayActuator {
  public:
    Actuator(
      int posPerThousandAccuracy,
      float posInputMinVolts,
      float posInputMaxVolts,
      byte posInputPin,
      byte isTotallyFoldedInputPin,
      byte relaySourceFoldPin,
      byte relaySourceUnfoldPin,
      byte relayMotorPin1,
      byte relayMotorPin2
    ) : RelayActuator(
      posPerThousandAccuracy,
      posInputMinVolts,
      posInputMaxVolts,
      posInputPin,
      isTotallyFoldedInputPin,
      relaySourceFoldPin,
      relaySourceUnfoldPin,
      relayMotorPin1,
      relayMotorPin2
    ) {};

  protected:
    /*
     * relaySourceFold est par defaut connecte au (+)
     * relaySourceUnfold est par defaut connecte au (-)
     */

    void _setSourceRelaysForFolding() {
      // On place les relais dans leurs positions par defaut pour envoyer du
      // (+) avec relaySourceFold et du (-) avec relaySourceUnfold.
      digitalWrite(_relaySourceFoldPin, LOW);
      digitalWrite(_relaySourceUnfoldPin, LOW);
    };

    void _setSourceRelaysForUnfolding() {
      // On active les relais pour envoyer du (-) avec relaySourceFold et du
      // (+) avec relaySourceUnfold.
      digitalWrite(_relaySourceFoldPin, HIGH);
      digitalWrite(_relaySourceUnfoldPin, HIGH);
    };

    /*
     * relayMotorPin1 est par defaut ouvert, le courant ne passe pas
     * relayMotorPin2 est par defaut ouvert, le courant ne passe pas
     */

    void _connectMotorRelays() {
      // On actionne les relais pour les fermer et connecter l'electrovanne au courant.
      digitalWrite(_relayMotorPin1, HIGH);
      digitalWrite(_relayMotorPin2, HIGH);
    };

    void _disconnectMotorRelays() {
      // On desactive les relais pour les ouvrir et couper le courant de l'electrovanne.
      digitalWrite(_relayMotorPin1, LOW);
      digitalWrite(_relayMotorPin2, LOW);
    };
};

Actuator actuatorLeft(
  PRECISION_POSITION_POUR_MILLE,
  VERIN_G_MIN_VOLTS,
  VERIN_G_MAX_VOLTS,
  VERIN_G_PIN_ENTREE,
  VERIN_G_PIN_FIN_COURSE,
  VERIN_G_PIN_R1,
  VERIN_G_PIN_R2,
  VERIN_G_PIN_R3,
  VERIN_G_PIN_R4
);

Actuator actuatorRight(
  PRECISION_POSITION_POUR_MILLE,
  VERIN_D_MIN_VOLTS,
  VERIN_D_MAX_VOLTS,
  VERIN_D_PIN_ENTREE,
  VERIN_D_PIN_FIN_COURSE,
  VERIN_D_PIN_R1,
  VERIN_D_PIN_R2,
  VERIN_D_PIN_R3,
  VERIN_D_PIN_R4
);

Knob targetPosKnob(
  POTAR_MIN_VOLTS,
  POTAR_MAX_VOLTS,
  POTAR_PIN_ENTREE
);

void setup() {
  actuatorLeft.stop();
  actuatorRight.stop();
}

void loop() {
  /*
  Pour avoir des deplacements des verins "simultanes" (afin d'eviter de trop grands
  ecarts de positions), on les deplace l'un apres l'autre petit a petit.

  Le fait de lire la position cible apres chaque petit deplacement permet
  de prendre en compte un changement de cible en cours de route.
  Si on attendait que les deplacements soient termines avant de lire la nouvelle
  cible, on pourrait se retrouver a faire des allers-retours. Par exemple :

   1. Position courante : d = 20cm
   2. Lecture de la position cible : d = 2cm
   3. Deplacement a d = 19cm
   4. Deplacement a d = 18cm
   5. Deplacement a d = 17cm
   6. Changement de position cible : d = 10cm (non detecte car le deplacement
      precedent n'est pas termine)
   7. Deplacement a d = 16cm
   8. ...
   9. Deplacement a d = 2cm
  10. Lecture de la position cible : d = 10cm
  11. Deplacement a d = 3cm
  12. Deplacement a d = 4cm
  13. ...
  14. Deplacement a d = 10cm
  15. Lecture de la position cible : d = 10cm
  16. Position cible deja atteinte, rien a faire
  
  Au lieu de :
  
   1. Position courante : d = 20cm
   2. Lecture de la position cible : d = 2cm
   3. Deplacement a d = 19cm
   4. Deplacement a d = 18cm
   5. Deplacement a d = 17cm
   6. Changement de position cible : d = 10cm
   7. Deplacement a d = 16cm
   8. ...
   9. Deplacement a d = 10cm
  10. Lecture de la position cible : d = 10cm
  11. Position cible deja atteinte, rien a faire

  Travailler avec des positions relatives au niveau du potentiometre permet de
  rester dans le domaine des tensions et de ne pas avoir a faire des conversions
  en distance.
  Cela suppose que la position cible relative est la meme pour les deux verins,
  et donc qu'ils sont de la meme longueur si on veut qu'une meme position relative
  corresponde a deux positions absolues identiques.
  */
  float targetPosRatio = targetPosKnob.readTargetPosRatio();
  actuatorLeft.startMovingTo(targetPosRatio);
  actuatorRight.startMovingTo(targetPosRatio);
}
