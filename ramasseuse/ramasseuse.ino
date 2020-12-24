#include "src/I2CRelayActuator.h"
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

// Alerte
// Si le verin met plus que x fois la duree de deplacement prevue, on lance une
// alerte.
#define VERIN_COEF_DUREE_DEPLACEMENT_ALERTE 2
#define BUZZER_PIN_SORTIE 2

// Verin gauche
#define VERIN_G_MIN_VOLTS 0
#define VERIN_G_MAX_VOLTS 5
#define VERIN_G_PIN_ENTREE 0
#define VERIN_G_PIN_FIN_COURSE_REPLIE 0
#define VERIN_G_PIN_FIN_COURSE_DEPLIE 0
#define VERIN_G_RELAIS_ADR_I2C 0x11
#define VERIN_G_DUREE_MAX_DEPLACEMENT 0
#define VERIN_G_ETAT_RELAIS_STOP 0
#define VERIN_G_ETAT_RELAIS_REPLIER CHANNLE3_BIT | CHANNLE4_BIT
#define VERIN_G_ETAT_RELAIS_DEPLIER CHANNLE1_BIT | CHANNLE2_BIT | CHANNLE3_BIT | CHANNLE4_BIT

// Verin droit
#define VERIN_D_MIN_VOLTS 0
#define VERIN_D_MAX_VOLTS 5
#define VERIN_D_PIN_ENTREE 0
#define VERIN_D_PIN_FIN_COURSE_REPLIE 0
#define VERIN_D_PIN_FIN_COURSE_DEPLIE 0
#define VERIN_D_RELAIS_ADR_I2C 0x21
#define VERIN_D_DUREE_MAX_DEPLACEMENT 0
#define VERIN_D_ETAT_RELAIS_STOP 0
#define VERIN_D_ETAT_RELAIS_REPLIER CHANNLE3_BIT | CHANNLE4_BIT
#define VERIN_D_ETAT_RELAIS_DEPLIER CHANNLE1_BIT | CHANNLE2_BIT | CHANNLE3_BIT | CHANNLE4_BIT


I2CRelayActuator actuatorLeft(
  PRECISION_POSITION_POUR_MILLE,
  VERIN_G_MIN_VOLTS,
  VERIN_G_MAX_VOLTS,
  VERIN_G_PIN_ENTREE,
  VERIN_G_PIN_FIN_COURSE_REPLIE,
  VERIN_G_PIN_FIN_COURSE_DEPLIE,
  VERIN_G_DUREE_MAX_DEPLACEMENT,
  VERIN_COEF_DUREE_DEPLACEMENT_ALERTE,
  VERIN_G_RELAIS_ADR_I2C,
  VERIN_G_ETAT_RELAIS_STOP,
  VERIN_G_ETAT_RELAIS_REPLIER,
  VERIN_G_ETAT_RELAIS_DEPLIER
);

I2CRelayActuator actuatorRight(
  PRECISION_POSITION_POUR_MILLE,
  VERIN_D_MIN_VOLTS,
  VERIN_D_MAX_VOLTS,
  VERIN_D_PIN_ENTREE,
  VERIN_D_PIN_FIN_COURSE_REPLIE,
  VERIN_D_PIN_FIN_COURSE_DEPLIE,
  VERIN_D_DUREE_MAX_DEPLACEMENT,
  VERIN_COEF_DUREE_DEPLACEMENT_ALERTE,
  VERIN_D_RELAIS_ADR_I2C,
  VERIN_D_ETAT_RELAIS_STOP,
  VERIN_D_ETAT_RELAIS_REPLIER,
  VERIN_D_ETAT_RELAIS_DEPLIER
);

Knob targetPosKnob(
  POTAR_MIN_VOLTS,
  POTAR_MAX_VOLTS,
  POTAR_PIN_ENTREE
);

bool isModeAuto = true;

void raiseAlert() {
  digitalWrite(BUZZER_PIN_SORTIE, HIGH);
}

void setup() {
  actuatorLeft.stop();
  actuatorRight.stop();

  pinMode(BUZZER_PIN_SORTIE, OUTPUT);
  digitalWrite(BUZZER_PIN_SORTIE, LOW);
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
  if (!isModeAuto) {
    return;
  }
  float targetPosRatio = targetPosKnob.readTargetPosRatio();
  actuatorLeft.startMovingTo(targetPosRatio);
  actuatorRight.startMovingTo(targetPosRatio);
  if (actuatorLeft.looksBlocked() || actuatorRight.looksBlocked()) {
    actuatorLeft.stop();
    actuatorRight.stop();
    raiseAlert();
    isModeAuto = false;
  }
}
