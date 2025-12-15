#include "game.h"
#include <stdio.h>
#include <stdlib.h>

int difficulty = 0; // 0 = easy, 1 = hard
int score = 0; // Score de la partie en cours
int highscore = 0; // Meilleur score

void chargerHighscore() {
    /*
     * Récupérer le highscore depuis le fichier "highscore.txt"
     */
    FILE *fichier = fopen("highscore.txt", "r");
    if (!fichier) return;
    fscanf(fichier, "%d", &highscore);
    fclose(fichier);
}

void sauvegarderHighscore() {
    /*
     * Sauvegarder le highscore dans le fichier "highscore.txt"
     */
    FILE *fichier = fopen("highscore.txt", "w");
    if (!fichier) return;
    fprintf(fichier, "%d", highscore);
    fclose(fichier);
}

/* -- Logique 2048 -- */
void inverserLigne(int ligne[4]) {
    /*
     * Inverser une ligne de la grille
     */
    for (int i = 0; i < 2; i++) {
        int tmp = ligne[i];
        ligne[i] = ligne[3 - i];
        ligne[3 - i] = tmp;
    }
}

void transposerGrille(int g[4][4]) {
    /*
     * Transposer la grille (échange lignes/colonnes)
     */
    for (int i = 0; i < 4; i++)
        for (int j = i + 1; j < 4; j++) {
            int tmp = g[i][j];
            g[i][j] = g[j][i];
            g[j][i] = tmp;
        }
}

void ajouterTuile(int grille[4][4]) {
    /*
     * Ajouter une tuile (2 ou 4) à une position aléatoire vide de la grille
     */
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
    /*
     * Déplacer les tuiles vers la gauche et fusionner les tuiles identiques si possible
     */
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
    /*
     * Déplacer les tuiles vers la droite et fusionner les tuiles identiques si possible
     * Utilise la fonction de déplacement gauche en inversant les lignes avant et après
     */
    for (int i = 0; i < 4; i++) inverserLigne(grille[i]);
    deplacerGauche(grille, updateScore);
    for (int i = 0; i < 4; i++) inverserLigne(grille[i]);
}
void deplacerHaut(int grille[4][4], int updateScore) {
    /*
     * Déplacer les tuiles vers le haut et fusionner les tuiles identiques si possible
     * Utilise la fonction de déplacement gauche en transposant la grille avant et après
     */
    transposerGrille(grille);
    deplacerGauche(grille, updateScore);
    transposerGrille(grille);
}
void deplacerBas(int grille[4][4], int updateScore) {
    /*
     * Déplacer les tuiles vers le bas et fusionner les tuiles identiques si possible
     * Utilise la fonction de déplacement gauche en transposant la grille et en inversant les lignes avant et après
     */
    transposerGrille(grille);
    for (int i = 0; i < 4; i++) inverserLigne(grille[i]);
    deplacerGauche(grille, updateScore);
    for (int i = 0; i < 4; i++) inverserLigne(grille[i]);
    transposerGrille(grille);
}

int grillesIdentiques(int a[4][4], int b[4][4]) {
    /*
     * Renvoie 1 si les grilles a et b sont identiques, 0 sinon
     */
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            if (a[i][j] != b[i][j]) return 0;
    return 1;
}

void copierGrille(int src[4][4], int dest[4][4]) {
    /*
     * Copier la grille src dans la grille dest
     */
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++) dest[i][j] = src[i][j];
}

int mouvementPossible(int grille[4][4]) {
    /*
     * Vérifier si un mouvement est possible dans la grille
     */
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