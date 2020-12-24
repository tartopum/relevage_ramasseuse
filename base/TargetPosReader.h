#ifndef TargetPosReader_h
#define TargetPosReader_h

class TargetPosReader {
  public:
    // Le capteur lisant la position cible ne retourne que des positions sous
    // forme de ratio.
    // Comme ca, la conversion ratio -> distance ne se fait qu'au niveau
    // de la classe qui commande les verins.
    virtual int readTargetPos() = 0;
};

#endif
