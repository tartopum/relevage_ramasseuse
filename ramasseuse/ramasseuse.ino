#include "src/I2CRelayActuator.h"
#include "src/Knob.h"

/*
 * Variables
 */

// ************
// Potar de cabine
// ************
#define POTAR_PIN A1
#define POTAR_VAL_DEPLIE 0
#define POTAR_VAL_REPLIE 1023
#define POTAR_VAL_BRUIT 2
#define POTAR_DELAI_LECTURE 1000 // ms

// ************
// Alerte
// ************
// On lance une alerte si la vitesse de deplacement d'un verin est inferieure a
// une certaine valeur.
// Les vitesses sont celles de deplacemenent du verin (et non de la hauteur du
// rouleau), en mm/s.
#define VERIN_G_VITESSE_MIN 4 // mm/s
#define VERIN_D_VITESSE_MIN 4 // mm/s
#define VERIN_PERIODE_CHECK 1000 // ms
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
#define VERIN_D_ETAT_RELAIS_DEPLIER CHANNLE3_BIT | CHANNLE4_BIT
#define VERIN_D_ETAT_RELAIS_REPLIER CHANNLE1_BIT | CHANNLE2_BIT | CHANNLE3_BIT | CHANNLE4_BIT

// Le rouleau des verins a une amplitude verticale d'environ 60mm.
// On veut une precision de sa longueur a 1mm, soit 16pm de 60mm.
#define VERIN_PRECISION_LONGUEUR 16 // pm


I2CRelayActuator actuatorLeft(
  VERIN_PRECISION_LONGUEUR,
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
  VERIN_PRECISION_LONGUEUR,
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
  POTAR_VAL_BRUIT,
  POTAR_DELAI_LECTURE
);

bool alertRaised = false;

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
  // Tout d'abord, on arrete les verins au besoin (cible atteinte, fin de course
  // ou blocage).
  actuator_stop_reason_t stopReasonLeft = actuatorLeft.stopIfNecessary();
  actuator_stop_reason_t stopReasonRight = actuatorRight.stopIfNecessary();

  // Si un verin est bloque (probablement parce que quelque chose est coince dessous),
  // on arrete les deux verins et lance une alerte pour que l'usager puisse
  // regler le probleme.
  if (stopReasonLeft == STOP_BLOCKED || stopReasonRight == STOP_BLOCKED) {
    actuatorLeft.stop();
    actuatorRight.stop();
    raiseAlert();
  }

  int targetLen = targetLenKnob.readTargetLen();

  // Pour arreter l'alarme, l'usager place le potentiometre de cabine en position
  // replie, de sorte a liberer ce qui est coince sous le verin.
  // Pour que cette instruction fonctionne, il faut que le potentiometre de
  // cabine retourne bien 0 quand il est en position extreme replie, et non pas
  // une petite valeur positive due a une imprecision.
  if (targetLen == 0) {
    stopAlert();
  }

  // Si la longueur cible a change, on indique aux verins de lancer le deplacement
  // vers la nouvelle cible. Une fois sa cible atteinte, le verin s'arrete et
  // attend une nouvelle position cible.
  //
  // Comme la longueur cible n'est retournee qu'une seule fois par targetLenKnob
  // (le reste du temps, il retourne NO_TARGET_LEN_CHANGE), le verin ne cherchera
  // pas en boucle a atteindre la meme position cible. Cela implique que s'il s'est
  // arrete a cote de sa cible parce que son capteur de position a retourne une
  // valeur invalide, il ne corrigera pas sa position de lui-meme. L'usager devra
  // changer de position cible pour que le verin se deplace de nouveau.
  if (targetLen != NO_TARGET_LEN_CHANGE && !alertRaised) {
    actuatorLeft.startMovingTo(targetLen);
    actuatorRight.startMovingTo(targetLen);
  }
}
