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
// Les vitesses sont en pour-mille/s
// Si le verin fait 10cm et qu'on veut lancer une alerte si sa vitesse de deplacement
// est en-dessous de 1cm/s, on a :
//   10cm = 1000pm
//   donc 1cm = 100pm
//   donc vitesse min = 100
#define VERIN_G_VITESSE_MIN 100
#define VERIN_D_VITESSE_MIN 100
#define BUZZER_PIN_SORTIE 2

// Verin gauche
#define VERIN_G_MIN_VOLTS 0
#define VERIN_G_MAX_VOLTS 5
#define VERIN_G_PIN_ENTREE 0
#define VERIN_G_PIN_FIN_COURSE_REPLIE 0
#define VERIN_G_PIN_FIN_COURSE_DEPLIE 0
#define VERIN_G_RELAIS_ADR_I2C 0x11
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
  VERIN_G_VITESSE_MIN,
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
  VERIN_D_VITESSE_MIN,
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

bool alertRaised = false;
int prevTargetPos;

void raiseAlert() {
  alertRaised = true;
  digitalWrite(BUZZER_PIN_SORTIE, HIGH);
}

void stopAlert() {
  alertRaised = false;
  digitalWrite(BUZZER_PIN_SORTIE, LOW);
}

void setup() {
  actuatorLeft.stop();
  actuatorRight.stop();

  pinMode(BUZZER_PIN_SORTIE, OUTPUT);
  stopAlert();
}

void loop() {
  /*
  Travailler avec des positions relatives au niveau du potentiometre permet de
  rester dans le domaine des tensions et de ne pas avoir a faire des conversions
  en distance.
  Cela suppose que la position cible relative est la meme pour les deux verins,
  et donc qu'ils sont de la meme longueur si on veut qu'une meme position relative
  corresponde a deux positions absolues identiques.
  */
  int targetPos = targetPosKnob.readTargetPos();

  if (abs(targetPos - prevTargetPos) > PRECISION_POSITION_POUR_MILLE) {
    // Un humain est intervenu sur le potentiometre de cabine, on arrete l'alerte.
    // On suppose en effet qu'il sait ce qu'il fait et commande les verins pour
    // regler le probleme, si probleme il y a.
    stopAlert();
  }

  prevTargetPos = targetPos;

  if (actuatorLeft.looksBlocked() || actuatorRight.looksBlocked()) {
    actuatorLeft.stop();
    actuatorRight.stop();
    raiseAlert();
  }
  if (!alertRaised) {
    actuatorLeft.startMovingTo(targetPos);
    actuatorRight.startMovingTo(targetPos);
  }
}
