#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void afficherGrille(int grille[4][4]) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (grille[i][j] == 0)
                printf("_\t");
            else
                printf("%d\t", grille[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void inverserLigne(int ligne[4]) {
    for (int i = 0; i < 2; i++) {
        int temp = ligne[i];
        ligne[i] = ligne[3 - i];
        ligne[3 - i] = temp;
    }
}

void transposerGrille(int grille[4][4]) {
    for (int i = 0; i < 4; i++) {
        for (int j = i + 1; j < 4; j++) {
            int temp = grille[i][j];
            grille[i][j] = grille[j][i];
            grille[j][i] = temp;
        }
    }
}


void ajouterTuile(int grille[4][4]) {
    int pos_vide[16][2];
    int nb_vides = 0;

    // chercher les cases vides
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (grille[i][j] == 0) {
                pos_vide[nb_vides][0] = i;
                pos_vide[nb_vides][1] = j;
                nb_vides++;
            }
        }
    }

    // si la case est vide, on génère aléatoirement un 2 ou un 4 (80% un 2 | 20 % un 4)
    if (nb_vides > 0) {
        int choix = rand() % nb_vides;
        int x = pos_vide[choix][0];
        int y = pos_vide[choix][1];

        int val = (rand() % 5 == 0) ? 4 : 2;
        grille[x][y] = val;
    }
}

void deplacerGauche(int grille[4][4]) {
    for (int i = 0; i < 4; i++) {
        // deplacer les tuiles vers la gauche
        int temp[4] = {0};
        int pos = 0;
        for (int j = 0; j < 4; j++) {
            if (grille[i][j] != 0) {
                temp[pos] = grille[i][j];
                pos++;
            }
        }

        // fusionner les tuiles, sur la même ligne, de même valeur, après le déplacement vers la gauche
        for (int j = 0; j < 3; j++) {
            if (temp[j] != 0 && temp[j] == temp[j + 1]) {
                temp[j] *= 2;
                temp[j + 1] = 0;
            }
        }

        // deplacer toutes les tuiles vers la gauche pour ne pas laisser de trous
        int final[4] = {0};
        pos = 0;
        for (int j = 0; j < 4; j++) {
            if (temp[j] != 0) {
                final[pos] = temp[j];
                pos++;
            }
        }

        // mise à jour
        for (int j = 0; j < 4; j++) {
            grille[i][j] = final[j];
        }
    }
}

void deplacerDroite(int grille[4][4]) {
    for (int i = 0; i < 4; i++) {
        inverserLigne(grille[i]);
    }

    deplacerGauche(grille);

    for (int i = 0; i < 4; i++) {
        inverserLigne(grille[i]);
    }
}

void deplacerHaut(int grille[4][4]) {
    transposerGrille(grille);
    deplacerGauche(grille);
    transposerGrille(grille);
}

void deplacerBas(int grille[4][4]) {
    transposerGrille(grille);

    for (int i = 0; i < 4; i++) {
        inverserLigne(grille[i]);
    }

    deplacerGauche(grille);

    for (int i = 0; i < 4; i++) {
        inverserLigne(grille[i]);
    }

    transposerGrille(grille);
}

int grillesIdentiques(int a[4][4], int b[4][4]) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (a[i][j] != b[i][j]) {
                return 0; // grilles differentes
            }
        }
    }
    return 1; // grilles identiques
}

void copierGrille(int src[4][4], int dest[4][4]) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            dest[i][j] = src[i][j];
        }
    }
}

int mouvementPossible(int grille[4][4]) {
    int temp[4][4];

    copierGrille(grille, temp);
    deplacerGauche(temp);
    if (!grillesIdentiques(grille, temp)) return 1;

    copierGrille(grille, temp);
    deplacerDroite(grille);
    if (!grillesIdentiques(grille, temp)) return 1;

    copierGrille(grille, temp);
    deplacerHaut(temp);
    if (!grillesIdentiques(grille, temp)) return 1;

    copierGrille(grille, temp);
    deplacerBas(temp);
    if (!grillesIdentiques(grille, temp)) return 1;

    return 0;
}


int main() {
    int grille[4][4] = {0};
    int ancienne[4][4];

    srand(time(NULL));

    for (int i = 0; i < 2; i++) {
        ajouterTuile(grille);
    }

    char commande;

    while (1) {
        afficherGrille(grille);

        if (!mouvementPossible(grille)) {
            printf("Fin de jeu, aucun mouvement possible restant");
            break;
        }

        printf("Commande (z: haut, q: gauche, s: bas, d: droite, x: quitter) : ");
        scanf(" %c", &commande);

        copierGrille(grille, ancienne);

        switch (commande) {
            case 'z': deplacerHaut(grille);
                break;
            case 'q': deplacerGauche(grille);
                break;
            case 's': deplacerBas(grille);
                break;
            case 'd': deplacerDroite(grille);
                break;
            case 'x': printf("Sortie du jeu");
                return 0;
            default: printf("Commande invalide");
                continue;
        }

        if (!grillesIdentiques(grille, ancienne)) {
            ajouterTuile(grille);
        }
    }

    return 0;
}
