#ifndef TargetLenReader_h
#define TargetLenReader_h

class TargetLenReader {
  public:
    // Le capteur lisant la position cible ne retourne que des positions sous
    // forme de ratio.
    // Comme ca, la conversion ratio -> distance ne se fait qu'au niveau
    // de la classe qui commande les verins.
    virtual int readTargetLen() = 0;
};

#endif
