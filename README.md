# 2048 – Jeu en ligne de commande (C)

Ce projet est une version graphique du jeu **2048**, développée en langage **C**.  
Le but du jeu est simple : combiner des tuiles comportant des puissances de 2, jusqu'à atteindre la tuile **2048**.  
Il se joue dans une grille de 4x4 cases, et chaque mouvement fait apparaître une nouvelle tuile.

---

## 🎮 Comment jouer ?

Lorsque vous lancez le programme, une grille vide apparaît avec **deux tuiles** (2 ou 4) placées aléatoirement.  
Vous pouvez ensuite **déplacer toutes les tuiles** dans l’une des quatre directions en utilisant les touches suivantes :

- `z` pour déplacer vers le haut
- `q` pour déplacer vers la gauche
- `s` pour déplacer vers le bas
- `d` pour déplacer vers la droite

Quand deux tuiles ayant la **même valeur** se rencontrent lors d’un déplacement, elles **fusionnent** en une seule tuile 
récupérant la **somme** des tuiles précédentes (par exemple, 2 + 2 = 4).  
Après chaque déplacement, une **nouvelle tuile** (2 ou 4) est ajoutée aléatoirement dans une case vide sur la grille.

---

## 🏆 Objectif du jeu

Le but est d’atteindre la **tuile 2048**.  
Lorsque vous y parvenez, le jeu continue, essayez de faire le plus de score possible.  

---

## 💀 Fin de partie

La partie se termine si la grille est **pleine**.
C'est-à-dire qu'aucun déplacement n'est possible pour effectuer une fusion ou un déplacement.
Lorsque vous arriver à cette étape, le jeu s'arrête et revient à l'accueil, vous avez perdu.

---

## ⚙️ Fonctionnement technique

Le jeu fonctionne avec une **interface graphique**.  
L'interface se lance et affiche un menu permettant à l'utilisateur de choisir différentes options telles que : jouer, changer la difficulté, quitter ou afficher les instructions.

Le jeu fonctionne comme suit:

1. Lecture de la touche appuyée par le joueur (z, q, s, d, x). (z: haut, q: gauche, s: bas, d: droite, x: quitter)
2. Application du déplacement.
3. Fusion éventuelle des tuiles identiques.
4. Ajout d’une tuile (si le déplacement a modifié la grille).
5. Augmente le score du joueur 

---

## 📦 Lancer le jeu

Compiler le fichier source avec `gcc` :

```bash
gcc -o jeu2048 jeu2048.c
./jeu2048
