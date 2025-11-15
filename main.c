#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define WIDTH 400  // largeur de la fenêtre
#define HEIGHT 500   // hauteur de la fenêtre
#define CELL  (WIDTH/4)  // taille d'une cellule
int difficulty = 0;
int score = 0;
int highscore = 0;

// highscore
void chargerHighscore() {
    FILE *fichier = fopen("highscore.txt", "r");
    if (!fichier) return;
    fscanf(fichier, "%d", &highscore);
    fclose(fichier);
}

// Sauvegarder le highscore
void sauvegarderHighscore() {
    FILE *fichier = fopen("highscore.txt", "w");
    if (!fichier) return;
    fprintf(fichier, "%d", highscore);
    fclose(fichier);
}

// ----------------------------------------------------
// Fonctions à retirer plus tard (debug)
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
    for (int i = 0; i < 4; i++)
        for (int j = i + 1; j < 4; j++) {
            int temp = grille[i][j];
            grille[i][j] = grille[j][i];
            grille[j][i] = temp;
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

void deplacerGauche(int grille[4][4]) {
    for (int i = 0; i < 4; i++) {
        int temp[4] = {0}, pos = 0;
        for (int j = 0; j < 4; j++)
            if (grille[i][j] != 0) temp[pos++] = grille[i][j];

        for (int j = 0; j < 3; j++)
            if (temp[j] != 0 && temp[j] == temp[j + 1]) {
                temp[j] *= 2;
                score += temp[j];
                temp[j + 1] = 0;
                if (score > highscore) highscore = score;
            }

        int final[4] = {0};
        pos = 0;
        for (int j = 0; j < 4; j++)
            if (temp[j] != 0) final[pos++] = temp[j];

        for (int j = 0; j < 4; j++) grille[i][j] = final[j];
    }
}

void deplacerDroite(int grille[4][4]) {
    for (int i = 0; i < 4; i++) inverserLigne(grille[i]);
    deplacerGauche(grille);
    for (int i = 0; i < 4; i++) inverserLigne(grille[i]);
}

void deplacerHaut(int grille[4][4]) {
    transposerGrille(grille);
    deplacerGauche(grille);
    transposerGrille(grille);
}

void deplacerBas(int grille[4][4]) {
    transposerGrille(grille);
    for (int i = 0; i < 4; i++) inverserLigne(grille[i]);
    deplacerGauche(grille);
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
    deplacerGauche(temp);
    if (!grillesIdentiques(grille, temp)) return 1;
    copierGrille(grille, temp);
    deplacerDroite(temp);
    if (!grillesIdentiques(grille, temp)) return 1;
    copierGrille(grille, temp);
    deplacerHaut(temp);
    if (!grillesIdentiques(grille, temp)) return 1;
    copierGrille(grille, temp);
    deplacerBas(temp);
    if (!grillesIdentiques(grille, temp)) return 1;
    return 0;
}

SDL_Color colorForValue(int v) {
    if (v == 1) return (SDL_Color){248, 238, 228, 255};
    if (v == 2) return (SDL_Color){238, 228, 218, 255};
    if (v == 4) return (SDL_Color){237, 224, 200, 255};
    if (v == 8) return (SDL_Color){242, 177, 121, 255};
    if (v == 16) return (SDL_Color){245, 149, 99, 255};
    if (v == 32) return (SDL_Color){246, 124, 95, 255};
    if (v == 64) return (SDL_Color){246, 94, 59, 255};
    if (v > 64) return (SDL_Color){237, 207, 114, 255};
    return (SDL_Color){205, 193, 180, 255};
}

void afficherTexte(SDL_Renderer *renderer, TTF_Font *font, const char *texte, int x, int y) {
    SDL_Color noir = {50, 50, 50, 255};
    SDL_Surface *surface = TTF_RenderText_Blended(font, texte, noir);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect rect = {x - surface->w / 2, y - surface->h / 2, surface->w, surface->h};
    SDL_FreeSurface(surface);

    SDL_RenderCopy(renderer, texture, NULL, &rect);
    SDL_DestroyTexture(texture);
}

int clique(int mx, int my, SDL_Rect r) {
    return mx >= r.x && mx <= r.x + r.w && my >= r.y && my <= r.y + r.h;
}

int menu(SDL_Renderer *r, TTF_Font *font) {
    int running = 1;
    SDL_Event e;

    SDL_Rect btnInstructions = {175, 350, 50, 50};
    SDL_Rect btnPlay = {100, 100, 200, 60};
    SDL_Rect btnDiff = {90, 180, 220, 60};
    SDL_Rect btnQuit = {100, 260, 200, 60};

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) return -1;
            if (e.type == SDL_MOUSEBUTTONDOWN) {
                int mx = e.button.x, my = e.button.y;

                if (clique(mx, my, btnPlay)) {
                    return 1;
                }

                // bouton difficulté facile/difficile
                if (clique(mx, my, btnDiff)) {
                    difficulty = (difficulty == 0 ? 1 : 0);
                }

                // bouton quitter
                if (clique(mx, my, btnQuit)) {
                    return -1;
                }

                // bouton instructions
                if (clique(mx, my, btnInstructions)) {
                    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Instructions",
                                             "Utilisez les touches Z (haut), Q (gauche), S (bas), D (droite) pour deplacer les tuiles.\n"
                                             "Combinez les tuiles de meme valeur pour en creer de nouvelles et atteindre la tuile 2048 !\n"
                                             "Appuyez sur X pour quitter la partie en cours.",NULL);
                }
            }
        }

        SDL_SetRenderDrawColor(r, 230, 220, 210, 255); // couleur de fond
        SDL_RenderClear(r); // effacer l'écran

        SDL_SetRenderDrawColor(r, 180, 170, 160, 255); // couleur des boutons

        // dessiner les boutons
        SDL_RenderFillRect(r, &btnInstructions);
        SDL_RenderFillRect(r, &btnPlay);
        SDL_RenderFillRect(r, &btnDiff);
        SDL_RenderFillRect(r, &btnQuit);

        afficherTexte(r, font, "2048 en C", 200, 50); // titre
        afficherTexte(r, font, "Jouer", 200, 130); // bouton jouer

        // bouton difficulté facile/difficile
        if (difficulty == 0) afficherTexte(r, font, "Difficulte: Facile", 200, 210);
        else afficherTexte(r, font, "Difficulte: Difficile", 200, 210);

        afficherTexte(r, font, "Quitter", 200, 290); // bouton quitter

        afficherTexte(r, font, "?", 200, 375); // bouton instructions

        SDL_RenderPresent(r); // afficher le rendu

        SDL_Delay(16); // ~60 FPS
    }
}

void jouer(SDL_Renderer *renderer, TTF_Font *font) {
    int g[4][4] = {0}, old[4][4];
    chargerHighscore();
    score = 0;
    char txtScore[32];
    sprintf(txtScore, "Score: %d", score);
    char txtHighscore[32];
    sprintf(txtHighscore, "Highscore: %d", highscore);


    ajouterTuile(g);
    ajouterTuile(g);

    int running = 1;
    SDL_Event e;

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) return;
            if (e.type == SDL_KEYDOWN) {
                copierGrille(g, old);
                switch (e.key.keysym.sym) {
                    case SDLK_z: deplacerHaut(g);
                        break;
                    case SDLK_q: deplacerGauche(g);
                        break;
                    case SDLK_s: deplacerBas(g);
                        break;
                    case SDLK_d: deplacerDroite(g);
                        break;
                    case SDLK_x: return; // quitter la partie
                }

                if (!grillesIdentiques(g, old)) ajouterTuile(g);
            }
        }

        SDL_SetRenderDrawColor(renderer, 187, 173, 160, 255);
        SDL_RenderClear(renderer);

        afficherTexte(renderer, font, txtScore, WIDTH / 2, 30);
        afficherTexte(renderer, font, txtHighscore, WIDTH / 2, 80);

        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                SDL_Color c = colorForValue(g[i][j]);
                SDL_Rect r = {j * CELL + 5, i * CELL + 5 + 100,CELL - 10,CELL - 10};
                SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
                SDL_RenderFillRect(renderer, &r);

                if (g[i][j] != 0) {
                    char txt[8];
                    sprintf(txt, "%d", g[i][j]);
                    afficherTexte(renderer, font, txt, j * CELL + CELL / 2, i * CELL + CELL / 2 + 100);
                }
            }
        }

        SDL_RenderPresent(renderer);

        if (!mouvementPossible(g)) {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "2048", "Aucun mouvement possible",NULL);
            return;
        }

        SDL_Delay(16);
        sauvegarderHighscore();
    }
}

// ----------------------------------------------------

int main() {
    srand((unsigned) time(NULL));
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    SDL_Window *window = SDL_CreateWindow("2048",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,WIDTH,HEIGHT, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    TTF_Font *font = TTF_OpenFont("CHILLER.TTF", 40);
    if (!font) {
        printf("Erreur police %s\n",TTF_GetError());
        return 1;
    }

    while (1) {
        int choix = menu(renderer, font);
        if (choix == -1) break;
        if (choix == 1) jouer(renderer, font);
    }

    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return 0;
}
