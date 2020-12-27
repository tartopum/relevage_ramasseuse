#include <multi_channel_relay.h>

#define POTAR_PIN A1
#define PRECISION_POTAR_POUR_MILLE 3
#define POTAR_SEUIL_BAS 200
#define POTAR_SEUIL_HAUT 800

#define VERIN_PIN_POS A3
#define VERIN_VAL_DEPLIE 515
#define VERIN_VAL_REPLIE 935
#define VERIN_POS_MAX_DIFF 4

#define VERIN_RELAIS_ADR_I2C 0x21
#define VERIN_ETAT_RELAIS_STOP 0
#define VERIN_ETAT_RELAIS_DEPLIER CHANNLE3_BIT | CHANNLE4_BIT
#define VERIN_ETAT_RELAIS_REPLIER CHANNLE1_BIT | CHANNLE2_BIT | CHANNLE3_BIT | CHANNLE4_BIT


Multi_Channel_Relay relay;

int prevPos = -1;
unsigned long lastTime = 0;

unsigned long lastPrintMillis = 0;
const int PRINT_PERIOD = 3000;

void debug(String m) {
  if (lastPrintMillis == 0 || (millis() - lastPrintMillis) > PRINT_PERIOD) {
    Serial.println(m);
    lastPrintMillis = millis();
  }
}
  
void setup() {
  Serial.begin(9600);

  pinMode(POTAR_PIN, INPUT);
  pinMode(VERIN_PIN_POS, INPUT);

  relay.begin(VERIN_RELAIS_ADR_I2C);
}

void loop() {
  int pos = analogRead(VERIN_PIN_POS);

  /*
  if (pos <= VERIN_VAL_DEPLIE || pos >= VERIN_VAL_REPLIE) {
    relay.channelCtrl(VERIN_ETAT_RELAIS_STOP); 
  }

  int targetVal = analogRead(POTAR_PIN);
  if (targetVal >= POTAR_SEUIL_BAS && targetVal <= POTAR_SEUIL_HAUT) {
    relay.channelCtrl(VERIN_ETAT_RELAIS_STOP); 
  } else if (targetVal < POTAR_SEUIL_BAS && pos > VERIN_VAL_DEPLIE) {
    relay.channelCtrl(VERIN_ETAT_RELAIS_DEPLIER);
  } else if (targetVal > POTAR_SEUIL_HAUT && pos < VERIN_VAL_REPLIE) {
    relay.channelCtrl(VERIN_ETAT_RELAIS_REPLIER);
  }
  */

  // debug(String(pos));

  int diff = abs(pos - prevPos);
  Serial.println(diff);
  if (prevPos != -1 && diff > VERIN_POS_MAX_DIFF) {
    Serial.println("");
    Serial.println("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
    Serial.print("Diff anormale : |");
    Serial.print(pos);
    Serial.print(" - ");
    Serial.print(prevPos);
    Serial.print("| = ");
    Serial.println(diff);
    Serial.print("Duree entre les mesures : ");
    Serial.print(millis() - lastTime);
    Serial.println("ms");
    Serial.println("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
    Serial.println("");
  }

  prevPos = pos;
  lastTime = millis();
}
