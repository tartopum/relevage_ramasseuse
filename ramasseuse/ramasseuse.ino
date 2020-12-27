#include "src/I2CRelayActuator.h"
#include "src/Knob.h"

/*
 * Variables
 */

// ************
// Potar de cabine
// ************
#define POTAR_VAL_DEPLIE 0
#define POTAR_VAL_REPLIE 1023
#define POTAR_PIN A1
#define POTAR_BRUIT 2

// Pour eviter les micro-deplacements permanents dus a des micro-variations des
// capteurs, ce qui abimerait les verins, on definit une marge d'erreur acceptable.
// On definit cette valeur en pour mille pour avoir des nombres entiers.
// 5,5% = 55 pour mille
//
// Le rouleau des verins a une amplitude verticale de 60mm.
// On veut une precision de sa position a 1mm, soit 33pm de 60mm.
#define PRECISION_VERIN_POUR_MILLE 16
// On se premunit contre les petites variations du potentiometre de cabine
// On a vu pendant la calibration que les valeurs analogRead() du potentiometre
// de cabine variaient d'au plus 3, ce qui fait 3/1023*1000 = 2.9pm
#define PRECISION_POTAR_POUR_MILLE 3

// ************
// Alerte
// ************
// On lance une alerte si la vitesse de deplacement d'un verin est inferieure a
// une certaine valeur.
// Les vitesses sont celles de deplacemenent du verin (et non de la hauteur du
// rouleau), en mm/s.
#define VERIN_G_VITESSE_MIN 2 // mm/s
#define VERIN_D_VITESSE_MIN 2 // mm/s
#define VERIN_PERIODE_CHECK 2000 // ms
#define BUZZER_PIN_SORTIE 8

// ************
// Verin gauche
// ************
#define VERIN_G_LONGUEUR_UTILISEE 90 // mm

#define VERIN_G_PIN_POS A2
#define VERIN_G_VAL_DEPLIE 432
#define VERIN_G_VAL_REPLIE 871

#define VERIN_G_PIN_FIN_COURSE_REPLIE 0
#define VERIN_G_PIN_FIN_COURSE_DEPLIE 0

#define VERIN_G_RELAIS_ADR_I2C 0x11
#define VERIN_G_ETAT_RELAIS_STOP 0
#define VERIN_G_ETAT_RELAIS_DEPLIER CHANNLE3_BIT | CHANNLE4_BIT
#define VERIN_G_ETAT_RELAIS_REPLIER CHANNLE1_BIT | CHANNLE2_BIT | CHANNLE3_BIT | CHANNLE4_BIT

// ************
// Verin droit
// ************
#define VERIN_D_LONGUEUR_UTILISEE 90 // mm

#define VERIN_D_PIN_POS A3
#define VERIN_D_VAL_DEPLIE 515
#define VERIN_D_VAL_REPLIE 935

#define VERIN_D_PIN_FIN_COURSE_REPLIE 0
#define VERIN_D_PIN_FIN_COURSE_DEPLIE 0

#define VERIN_D_RELAIS_ADR_I2C 0x21
#define VERIN_D_ETAT_RELAIS_STOP 0
#define VERIN_D_ETAT_RELAIS_REPLIER CHANNLE3_BIT | CHANNLE4_BIT
#define VERIN_D_ETAT_RELAIS_DEPLIER CHANNLE1_BIT | CHANNLE2_BIT | CHANNLE3_BIT | CHANNLE4_BIT


I2CRelayActuator actuatorLeft(
  PRECISION_VERIN_POUR_MILLE,
  VERIN_G_VAL_REPLIE,
  VERIN_G_VAL_DEPLIE,
  VERIN_G_PIN_POS,
  VERIN_G_PIN_FIN_COURSE_REPLIE,
  VERIN_G_PIN_FIN_COURSE_DEPLIE,
  (float)VERIN_G_VITESSE_MIN / VERIN_G_LONGUEUR_UTILISEE * 1000,
  VERIN_PERIODE_CHECK,
  VERIN_G_RELAIS_ADR_I2C,
  VERIN_G_ETAT_RELAIS_STOP,
  VERIN_G_ETAT_RELAIS_REPLIER,
  VERIN_G_ETAT_RELAIS_DEPLIER
);

I2CRelayActuator actuatorRight(
  PRECISION_VERIN_POUR_MILLE,
  VERIN_D_VAL_REPLIE,
  VERIN_D_VAL_DEPLIE,
  VERIN_D_PIN_POS,
  VERIN_D_PIN_FIN_COURSE_REPLIE,
  VERIN_D_PIN_FIN_COURSE_DEPLIE,
  (float)VERIN_D_VITESSE_MIN / VERIN_D_LONGUEUR_UTILISEE * 1000,
  VERIN_PERIODE_CHECK,
  VERIN_D_RELAIS_ADR_I2C,
  VERIN_D_ETAT_RELAIS_STOP,
  VERIN_D_ETAT_RELAIS_REPLIER,
  VERIN_D_ETAT_RELAIS_DEPLIER
);

Knob targetLenKnob(
  POTAR_VAL_REPLIE,
  POTAR_VAL_DEPLIE,
  POTAR_PIN,
  POTAR_BRUIT
);

int prevTargetLen = -1;

unsigned long lastPrintMillis = 0;
const int PRINT_PERIOD = 2000;

void serialPrintLn(String m, bool print) {
  if (print) {
    Serial.println(m);
  }
}

void raiseAlert() {
  digitalWrite(BUZZER_PIN_SORTIE, HIGH);
}

void stopAlert() {
  digitalWrite(BUZZER_PIN_SORTIE, LOW);
}

void setup() {
  Serial.begin(9600);

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

  // TODO: debug
  bool print = false;
  if (lastPrintMillis == 0 || (millis() - lastPrintMillis) > PRINT_PERIOD) {
    print = true;
    lastPrintMillis = millis();
  }

  // TODO right as well
  if (!actuatorLeft.check()) {
    raiseAlert();
  }

  int targetLen = targetLenKnob.readTargetLen();

  int len = actuatorLeft.readLen();
  serialPrintLn("Longueur cible = ", print);
  serialPrintLn(String(actuatorLeft._targetLen), print);
  serialPrintLn("Longueur verin = ", print);
  serialPrintLn(String(len), print);
  serialPrintLn("diff = ", print);
  serialPrintLn(String(abs(len - actuatorLeft._targetLen)), print);
  serialPrintLn("", print);

  // On met le potentiometre de cabine en position repliee pour couper l'alarme.
  if (targetLen == 0) {
    stopAlert();
  }

  if (targetLen != NO_TARGET_LEN_CHANGE) {
    // Un humain est intervenu sur le potentiometre de cabine, on arrete l'alerte.
    // On suppose en effet qu'il sait ce qu'il fait et commande les verins pour
    // regler le probleme, si probleme il y a.
    prevTargetLen = targetLen;
    actuatorLeft.startMovingTo(targetLen);
    // actuatorRight.startMovingTo(targetLen);
  }
}
