# Vérins

Un ensemble de projets d'asservissement de vérins électriques.

## Installation

```
git clone git@github.com:tartopum/verins.git
cd verins
```

## Compilation

Tout d'abord, créer un lien symbolique vers le dossier `base/` :

```
cd ramasseuse # Ou un autre projet
ln -s ../base src
```

Le lien symbolique doit s'appeler `src` car l'IDE Arduino ne semble
[pas lire les autres sous-dossiers](https://forum.arduino.cc/index.php?topic=445230.15).

Puis ouvrir le fichier `.ino` du projet souhaité (ex : `ramasseuse/ramasseuse.ino`)
avec l'IDE Arduino et lancer la compilation.

## Création d'un projet

```
mkdir <projet>
cd <projet>
touch <projet>.ino
ln -s ../base src
```

Un projet est constitué de deux types de composants :

* Le capteur lisant la position cible, qui suit l'interface `base/TargetPosReader.h`
* Les vérins, objets d'une classe qui hérite de `base/BaseActuator.h`

Pour faciliter la configuration de ces objets, il est conseillé de définir les
variables en haut du `.ino` avec des `#define`.

Un exemple de projet est dans le dossier `ramasseuse/`.

Sauf si les composants (capteur et vérin) sont très spécifiques, il est conseillé
de définir leurs classes dans le dossier `base/` afin qu'ils soient facilement
réutilisables.
