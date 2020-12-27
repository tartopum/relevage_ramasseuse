/*
Represente un potentiometre (ou autre capteur analogique lineaire) lisant la
longueur cible des verins.

Pour simplifier les calculs et travailler avec des nombres entiers, la longueur
cible est comprise entre 0 (position extreme replie) et 1000 (position extreme deplie).
Le deplacement du verin est suppose lineaire : une longueur de 500 correspond donc
a la moitie de l'amplitude du deplacement possible du verin.

Selon le sens d'usage du capteur, sa valeur minimum/maximum correspond a une
longueur de 0 ou de 1000.
*/

#ifndef Knob_h
#define Knob_h

#include "Arduino.h"
#include "TargetLenReader.h"

const int NO_TARGET_LEN_CHANGE = -1;
const int MAX_TARGET_LEN = 1000;

class Knob: public TargetLenReader {
  public:
    Knob(
      int foldedInputVal,
      int unfoldedInputVal,
      byte pin,
      int inputValMaxNoise,
      unsigned long readDelay
    );

    // Cette methode retourne une longueur entre 0 (longueur du verin minimale)
    // et 1000 (longueur du verin maximale).
    // Si la valeur n'a pas change depuis le dernier appel, en dehors du bruit,
    // on retourne NO_TARGET_LEN_CHANGE.
    // Avant de retourner une longueur apres un changement significatif de valeur,
    // on attend un peu pour ignorer les hesitations/tremblements de l'usager
    // (cf. exemples dans l'implementation de la methode).
    int readTargetLen();

  private:
    byte _pin;

    // La valeur du capteur pour une position replie du verin.
    // Selon le sens d'usage du capteur, peut etre inferieure ou superieure a celle
    // de la position deplie.
    int _foldedInputVal;
 
    // La valeur du capteur pour une position deplie du verin.
    // Selon le sens d'usage du capteur, peut etre inferieure ou superieure a celle
    // de la position replie.
    int _unfoldedInputVal;

    // Pour determiner si la valeur a change, en ignorant le bruit.
    // -1 permet de detecter qu'on est a la premiere lecture et qu'il faut
    // initialiser la variable avant de calculer la difference avec la valeur
    // courante.
    int _prevInputVal = -1;

    // L'ecart maximal entre deux valeurs dues a du bruit.
    int _inputValMaxNoise;

    // On met en cache le nombre de valeurs possibles du capteur.
    // Selon l'usage qu'on fait du potentiometre, on ne va pas de 0 a 1023.
    // Permet de faire le produit en croix pour convertir la valeur en longueur.
    // On utilise un float pour les divisions.
    float _inputValScale;

    // Le nombre de ms attendues avant de retourner la longueur calculee apres
    // un changement de valeur. Permet d'eviter les micro-deplacements dus aux
    // hesitations/tremblements de l'usager.
    unsigned long _readDelay;

    // Le moment du dernier changement de valeur, pour determiner s'il a eu lieu
    // il y a plus de _readDelay.
    unsigned long _lastChangeTime;

    // Pour ne retourner la valeur qu'une seule fois, apres que _readDelay soit passe.
    bool _valReturned = false;

    // Selon le sens d'utilisation du potentiometre, la position replie/deplie
    // correspond a la valeur minimum ou maximum du capteur.
    bool _isFoldedVal(int inputVal);
    bool _isUnfoldedVal(int inputVal);
};

#endif
