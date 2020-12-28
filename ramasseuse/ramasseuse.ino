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
#define POTAR_DELAI_LECTURE 800 // ms

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


class Actuator : public I2CRelayActuator {
  public:
    Actuator(
      int posAccuracy,
      int foldedInputVal,
      int unfoldedInputVal,
      byte posInputPin,
      byte isTotallyFoldedInputPin,
      byte isTotallyUnfoldedInputPin,
      int relayI2CAddr,
      uint8_t stopRelayState,
      uint8_t foldingRelayState,
      uint8_t unfoldingRelayState,
      int minSpeedAlert,
      unsigned checkPeriod
    ) : I2CRelayActuator(
      posAccuracy,
      foldedInputVal,
      unfoldedInputVal,
      posInputPin,
      isTotallyFoldedInputPin,
      isTotallyUnfoldedInputPin,
      relayI2CAddr,
      stopRelayState,
      foldingRelayState,
      unfoldingRelayState
    ) {
      _minSpeedAlert = minSpeedAlert;
      _checkPeriod = checkPeriod;
    };

  protected:
    // Pour detecter un blocage du verin, on s'assure que sa vitesse de deplacement
    // n'est pas inferieure a une certaine valeur.
    int _lastCheckLen = -1;
    unsigned long _lastCheckTime = 0;
    int _minSpeedAlert = 0;  // En pour-mille/s
    unsigned int _checkPeriod = 3000;

    bool _looksBlocked() {
      int len = readLen();
      unsigned long now = millis();
      unsigned long duration = now - _lastCheckTime;

      if (!_moving) {
        // On met a jour la date de verification, sinon au moment de relancer un
        // deplacement, la periode de verification sera passee et la vitesse sera
        // calculee sans moyenne.
        _lastCheckTime = now;
        _lastCheckLen = len;
        return false;
      }
      if (_lastCheckLen == -1) {
        _lastCheckTime = now;
        _lastCheckLen = len;
        return false;
      }

      // On attend que le deplacement ait dure un certain temps avant de calculer
      // la vitesse, sinon on est trop soumis aux petits aleas de deplacement.
      if (duration < _checkPeriod) {
        return false;
      }

      float movingTime = (float)duration / 1000;
      int speed = abs(len - _lastCheckLen) / movingTime;

      _lastCheckLen = len;
      _lastCheckTime = now;

      return speed <= _minSpeedAlert;
    };
};

Actuator actuatorLeft(
  VERIN_PRECISION_LONGUEUR,
  VERIN_G_VAL_REPLIE,
  VERIN_G_VAL_DEPLIE,
  VERIN_G_PIN_POS,
  VERIN_G_PIN_FIN_COURSE_REPLIE,
  VERIN_G_PIN_FIN_COURSE_DEPLIE,
  VERIN_G_RELAIS_ADR_I2C,
  VERIN_G_ETAT_RELAIS_STOP,
  VERIN_G_ETAT_RELAIS_REPLIER,
  VERIN_G_ETAT_RELAIS_DEPLIER,
  (float)VERIN_G_VITESSE_MIN / VERIN_G_LONGUEUR_UTILISEE * 1000,
  VERIN_PERIODE_CHECK
);

Actuator actuatorRight(
  VERIN_PRECISION_LONGUEUR,
  VERIN_D_VAL_REPLIE,
  VERIN_D_VAL_DEPLIE,
  VERIN_D_PIN_POS,
  VERIN_D_PIN_FIN_COURSE_REPLIE,
  VERIN_D_PIN_FIN_COURSE_DEPLIE,
  VERIN_D_RELAIS_ADR_I2C,
  VERIN_D_ETAT_RELAIS_STOP,
  VERIN_D_ETAT_RELAIS_REPLIER,
  VERIN_D_ETAT_RELAIS_DEPLIER,
  (float)VERIN_D_VITESSE_MIN / VERIN_D_LONGUEUR_UTILISEE * 1000,
  VERIN_PERIODE_CHECK
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
