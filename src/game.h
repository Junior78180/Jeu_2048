#ifndef GAME_H
#define GAME_H

extern int difficulty;
extern int score;
extern int highscore;

void chargerHighscore(void);
void sauvegarderHighscore(void);

void ajouterTuile(int grille[4][4]);
void deplacerGauche(int grille[4][4], int updateScore);
void deplacerDroite(int grille[4][4], int updateScore);
void deplacerHaut(int grille[4][4], int updateScore);
void deplacerBas(int grille[4][4], int updateScore);
int mouvementPossible(int grille[4][4]);
void copierGrille(int src[4][4], int dest[4][4]);

#endif // GAME_H