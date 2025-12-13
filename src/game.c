#include "game.h"
#include <stdio.h>
#include <stdlib.h>

int difficulty = 0;
int score = 0;
int highscore = 0;

void chargerHighscore() {
    FILE *fichier = fopen("highscore.txt", "r");
    if (!fichier) return;
    fscanf(fichier, "%d", &highscore);
    fclose(fichier);
}

void sauvegarderHighscore() {
    FILE *fichier = fopen("highscore.txt", "w");
    if (!fichier) return;
    fprintf(fichier, "%d", highscore);
    fclose(fichier);
}

/* -- Logique 2048 -- */
static void inverserLigne(int ligne[4]) {
    for (int i = 0; i < 2; i++) {
        int tmp = ligne[i];
        ligne[i] = ligne[3 - i];
        ligne[3 - i] = tmp;
    }
}

static void transposerGrille(int g[4][4]) {
    for (int i = 0; i < 4; i++)
        for (int j = i + 1; j < 4; j++) {
            int tmp = g[i][j];
            g[i][j] = g[j][i];
            g[j][i] = tmp;
        }
}

void ajouterTuile(int grille[4][4]) {
    int pos_vide[16][2];
    int nb_vides = 0;
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            if (grille[i][j] == 0) {
                pos_vide[nb_vides][0] = i;
                pos_vide[nb_vides][1] = j;
                nb_vides++;
            }

    if (nb_vides > 0) {
        int choix = rand() % nb_vides;
        int x = pos_vide[choix][0];
        int y = pos_vide[choix][1];
        if (difficulty == 1) {
            grille[x][y] = (rand() % 5 == 0) ? 4 : (rand() % 3 == 0) ? 2 : 1;
        } else {
            grille[x][y] = (rand() % 5 == 0) ? 4 : 2;
        }
    }
}

void deplacerGauche(int grille[4][4], int updateScore) {
    for (int i = 0; i < 4; i++) {
        int temp[4] = {0}, pos = 0;
        for (int j = 0; j < 4; j++)
            if (grille[i][j] != 0) temp[pos++] = grille[i][j];

        for (int j = 0; j < 3; j++)
            if (temp[j] != 0 && temp[j] == temp[j + 1]) {
                temp[j] *= 2;
                if (updateScore) {
                    score += temp[j];
                    if (score > highscore) highscore = score;
                }
                temp[j + 1] = 0;
            }

        int final[4] = {0};
        pos = 0;
        for (int j = 0; j < 4; j++)
            if (temp[j] != 0) final[pos++] = temp[j];

        for (int j = 0; j < 4; j++) grille[i][j] = final[j];
    }
}

void deplacerDroite(int grille[4][4], int updateScore) {
    for (int i = 0; i < 4; i++) inverserLigne(grille[i]);
    deplacerGauche(grille, updateScore);
    for (int i = 0; i < 4; i++) inverserLigne(grille[i]);
}
void deplacerHaut(int grille[4][4], int updateScore) {
    transposerGrille(grille);
    deplacerGauche(grille, updateScore);
    transposerGrille(grille);
}
void deplacerBas(int grille[4][4], int updateScore) {
    transposerGrille(grille);
    for (int i = 0; i < 4; i++) inverserLigne(grille[i]);
    deplacerGauche(grille, updateScore);
    for (int i = 0; i < 4; i++) inverserLigne(grille[i]);
    transposerGrille(grille);
}

int grillesIdentiques(int a[4][4], int b[4][4]) {
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            if (a[i][j] != b[i][j]) return 0;
    return 1;
}

void copierGrille(int src[4][4], int dest[4][4]) {
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++) dest[i][j] = src[i][j];
}

int mouvementPossible(int grille[4][4]) {
    int temp[4][4];
    copierGrille(grille, temp);
    deplacerGauche(temp, 0);
    if (!grillesIdentiques(grille, temp)) return 1;
    copierGrille(grille, temp);
    deplacerDroite(temp, 0);
    if (!grillesIdentiques(grille, temp)) return 1;
    copierGrille(grille, temp);
    deplacerHaut(temp, 0);
    if (!grillesIdentiques(grille, temp)) return 1;
    copierGrille(grille, temp);
    deplacerBas(temp, 0);
    if (!grillesIdentiques(grille, temp)) return 1;
    return 0;
}