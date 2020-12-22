#include "Actuator.h"
#include "Knob.h"

// Pour eviter les micro-deplacements permanents dus a des micro-variations des
// capteurs, ce qui abimerait les verins, on definit une marge d'erreur acceptable.
const int ACTUATOR_POS_PERTHOUSAND_ACCURACY = 50;

Actuator actuatorLeft(
);

Actuator actuatorRight(
);

Knob targetPosKnob();

void setup() {
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
  actuatorLeft.stepTo(targetPosRatio);
  actuatorRight.stepTo(targetPosRatio);
}
