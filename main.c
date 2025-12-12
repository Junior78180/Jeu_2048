#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define WIDTH 400  // largeur de la fenêtre
#define HEIGHT 500   // hauteur de la fenêtre
#define CELL  (WIDTH/4)  // taille d'une cellule
#define STARS 150
#define PLANETES 5
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


// Fond étoilé
typedef struct {
    float x, y;
    float vitesse;
    int taille;
} Etoile;

typedef struct {
    float x, y;
    float vitesse;
    int taille;
    SDL_Color couleur;
} Planete;

Etoile etoiles[STARS];
Planete planetes[PLANETES];

void initialiserEtoiles() {
    for (int i = 0; i < STARS; i++) {
        etoiles[i].x = rand() % WIDTH;
        etoiles[i].y = rand() % HEIGHT;
        etoiles[i].vitesse = 0.5f + (rand() % 10) / 80.0f;
        etoiles[i].taille = 1 + rand() % 2;
    }
}

void initialiserPlanetes() {
    for (int i = 0; i < PLANETES; i++) {
        planetes[i].x = rand() % WIDTH;
        planetes[i].y = rand() % HEIGHT;
        planetes[i].vitesse = 0.1f + (rand() % 100) / 200.0f;
        planetes[i].taille = 12 + rand() % 18;

        // Couleur
        planetes[i].couleur.r = 150 + rand() % 10;
        planetes[i].couleur.g = 80 + rand() % 120;
        planetes[i].couleur.b = 150 + rand() % 150;
        planetes[i].couleur.a = 180;
    }
}

void RemplirCercle(SDL_Renderer *r, int cx, int cy, int radius, SDL_Color color) {
    SDL_SetRenderDrawColor(r, color.r, color.g, color.b, color.a);

    for (int dy = -radius; dy <= radius; dy++) {
        int dxMax = (int) sqrt(radius * radius - dy * dy);
        SDL_RenderDrawLine(r, cx - dxMax, cy + dy, cx + dxMax, cy + dy);
    }
}

void drawPlanetGradient(SDL_Renderer *r, int cx, int cy, int radius, SDL_Color baseColor) {
    for (int i = 0; i < radius; i++) {
        float t = (float) i / radius;

        SDL_Color c = {
            (Uint8) (baseColor.r * (1.0f - 0.5f * t)),
            (Uint8) (baseColor.g * (1.0f - 0.5f * t)),
            (Uint8) (baseColor.b * (1.0f - 0.5f * t)),
            20
        };

        RemplirCercle(r, cx, cy, radius - i, c);
    }
}


void dessinerPlanetes(SDL_Renderer *r) {
    for (int i = 0; i < PLANETES; i++) {
        int cx = (int) planetes[i].x;
        int cy = (int) planetes[i].y;
        int rad = planetes[i].taille;

        // Dégradé (base sombre)
        drawPlanetGradient(r, cx, cy, rad, planetes[i].couleur);

        // Animation verticale
        planetes[i].y += planetes[i].vitesse;
        if (planetes[i].y > HEIGHT + rad) {
            planetes[i].x = rand() % WIDTH;
            planetes[i].y = -rad - 10;
        }
    }
}


void dessinerEtoiles(SDL_Renderer *r) {
    SDL_SetRenderDrawColor(r, 10, 10, 25, 255);
    SDL_RenderClear(r);

    SDL_SetRenderDrawColor(r, 255, 255, 255, 255);

    for (int i = 0; i < STARS; i++) {
        SDL_Rect rect = {(int) etoiles[i].x, (int) etoiles[i].y, etoiles[i].taille, etoiles[i].taille};
        SDL_RenderFillRect(r, &rect);
        etoiles[i].y += etoiles[i].vitesse;
        if (etoiles[i].y > HEIGHT) {
            etoiles[i].x = rand() % WIDTH;
            etoiles[i].y = 0;
        }
    }
}

// ----------------------------------------------------
// Logique du jeu 2048
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

// Fin logique du jeu 2048 ------------------------------

SDL_Color colorForValue(int v) {
    if (v == 1) return (SDL_Color){160, 150, 140, 200};
    if (v == 2) return (SDL_Color){150, 140, 130, 200};
    if (v == 4) return (SDL_Color){140, 120, 110, 200};
    if (v == 8) return (SDL_Color){150, 100, 70, 200};
    if (v == 16) return (SDL_Color){160, 80, 60, 200};
    if (v == 32) return (SDL_Color){170, 70, 55, 200};
    if (v == 64) return (SDL_Color){170, 55, 40, 200};
    if (v > 64) return (SDL_Color){150, 130, 60, 200};
    return (SDL_Color){110, 100, 90, 50};
}


void afficherTexte(SDL_Renderer *renderer, TTF_Font *font, const char *texte, int x, int y) {
    SDL_Color couleurTexte = {255, 255, 255, 255};
    SDL_Surface *surface = TTF_RenderText_Blended(font, texte, couleurTexte);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect rect = {x - surface->w / 2, y - surface->h / 2, surface->w, surface->h};
    SDL_FreeSurface(surface);

    SDL_RenderCopy(renderer, texture, NULL, &rect);
    SDL_DestroyTexture(texture);
}

void afficherTexteMultiligne(SDL_Renderer *renderer, TTF_Font *font, const char *texte, int x, int y) {
    if (!texte) return;
    char *texteCopy = strdup(texte);
    if (!texteCopy) return;
    int ligneY = TTF_FontHeight(font);
    int i = 0;
    char *ligne = strtok(texteCopy, "\n");
    while (ligne) {
        afficherTexte(renderer, font, ligne, x, y + i * ligneY + 4); // 4px d'espacement entre les lignes
        ligne = strtok(NULL, "\n");
        i++;
    }
    free(texteCopy);
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
                    return 0;
                }
            }
        }

        dessinerEtoiles(r); // dessiner le fond étoilé
        dessinerPlanetes(r);

        SDL_SetRenderDrawColor(r, 50, 50, 50, 150); // couleur des boutons

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
    return -1;
}

int instructions(SDL_Renderer *r, TTF_Font *font) {
    int running = 1;
    SDL_Event e;

    SDL_Rect fondTexteInstruction = {20, 120, 360, 280};
    SDL_Rect back = {175, 425, 50, 50};

    TTF_Font *small = TTF_OpenFont("CHILLER.ttf", 26);
    if (!small) {
        printf("Erreur police : %s\n", TTF_GetError());
        return -1;
    }

    const char *texte =
            "Utilisez les touches :\n"
            "Z (haut)\n"
            "Q (gauche)\n"
            "S (bas)\n"
            "D (droite)\n\n"
            "Combinez les tuiles de meme valeur !\n"
            "Creez des nouvelles !\n"
            "et atteignez la tuile 2048 !\n\n"
            "Appuyez sur X pour quitter la partie en cours";

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) return -1;
            if (e.type == SDL_MOUSEBUTTONDOWN) {
                int mx = e.button.x, my = e.button.y;

                // bouton instructions
                if (clique(mx, my, back)) {
                    // afficher les instructions
                    TTF_CloseFont(small);
                    return -1;
                }
            }
        }


        afficherTexte(r, font, "2048 en C", 200, 50); // titre

        dessinerEtoiles(r); // dessiner le fond étoilé
        dessinerPlanetes(r);
        afficherTexte(r, font, "Instructions :", 200, 90); // bouton jouer
        afficherTexteMultiligne(r, small, texte, 200, 130); // instructions
        SDL_SetRenderDrawColor(r, 50, 50, 50, 50); // couleur du fond des instructions
        SDL_RenderFillRect(r, &fondTexteInstruction);


        SDL_SetRenderDrawColor(r, 50, 50, 50, 150); // couleur des boutons
        // dessiner le bouton retour
        SDL_RenderFillRect(r, &back);
        afficherTexte(r, font, "X", 200, 450); // bouton retour

        SDL_RenderPresent(r); // afficher le rendu

        SDL_Delay(16); // ~60 FPS
    }
}

void jouer(SDL_Renderer *renderer, TTF_Font *font) {
    int g[4][4] = {0}, old[4][4];
    chargerHighscore();
    score = 0;
    int lastScore = highscore;
    // char txtScore[32];
    // sprintf(txtScore, "Score: %d", score);
    // char txtHighscore[32];
    // sprintf(txtHighscore, "Highscore: %d", highscore);


    ajouterTuile(g);
    ajouterTuile(g);

    int running = 1;
    SDL_Event e;
    char txtScore[32];
    char txtHighscore[32];

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                if (highscore != lastScore)
                    sauvegarderHighscore();
                return;
            }
            if (e.type == SDL_KEYDOWN) {
                copierGrille(g, old);
                switch (e.key.keysym.sym) {
                    case SDLK_z: deplacerHaut(g, 1);
                        break;
                    case SDLK_q: deplacerGauche(g, 1);
                        break;
                    case SDLK_s: deplacerBas(g, 1);
                        break;
                    case SDLK_d: deplacerDroite(g, 1);
                        break;
                    case SDLK_x: if (highscore != lastScore)
                            sauvegarderHighscore();
                        return; // quitter la partie et sauvegarder le highscore
                }

                if (!grillesIdentiques(g, old)) ajouterTuile(g);
            }
        }

        // Mettre à jour le score à chaque déplacement
        sprintf(txtScore, "Score: %d", score);
        sprintf(txtHighscore, "Highscore: %d", highscore);

        // Sauvegarder le highscore immédiatement si changé
        if (highscore != lastScore) {
            sauvegarderHighscore();
            lastScore = highscore;
        }

        dessinerEtoiles(renderer); // dessiner le fond étoilé
        dessinerPlanetes(renderer);

        afficherTexte(renderer, font, txtScore, WIDTH / 2, 30);
        afficherTexte(renderer, font, txtHighscore, WIDTH / 2, 80);

        // dessiner la grille, les tuiles et les nombres
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                // dessiner la case
                SDL_Color c = colorForValue(g[i][j]); // obtenir la couleur selon la valeur
                SDL_Rect r = {j * CELL + 5, i * CELL + 5 + 100,CELL - 10,CELL - 10}; // position et taille de la case
                SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a); // définir la couleur
                SDL_RenderFillRect(renderer, &r); // dessiner la case

                // afficher le nombre dans la case
                if (g[i][j] != 0) {
                    char txt[8];
                    sprintf(txt, "%d", g[i][j]);
                    afficherTexte(renderer, font, txt, j * CELL + CELL / 2, i * CELL + CELL / 2 + 100);
                }
            }
        }
        // printf("Score: %d, Highscore: %d\n", score, highscore);
        SDL_RenderPresent(renderer); // afficher le rendu

        if (!mouvementPossible(g)) {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "2048", "Aucun mouvement possible",NULL);
            if (highscore != lastScore) sauvegarderHighscore();
            return;
        }

        SDL_Delay(16);
    }
}

// ----------------------------------------------------

int main() {
    srand((unsigned) time(NULL));
    SDL_Init(SDL_INIT_VIDEO);
    initialiserEtoiles();
    initialiserPlanetes();
    TTF_Init();

    SDL_Window *window = SDL_CreateWindow("2048",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,WIDTH,HEIGHT, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    TTF_Font *font = TTF_OpenFont("CHILLER.TTF", 40);
    if (!font) {
        printf("Erreur police %s\n",TTF_GetError());
        return 1;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("Erreur Mix_OpenAudio: %s\n", Mix_GetError());
        return 1;
    }

    Mix_Music *musique = Mix_LoadMUS("musics/Interstellar_Main_Theme_Extra_Extended_Soundtrack_by_Hans_Zimmer.mp3");
    if (!musique) {
        printf("Erreur de chargement de la musique: %s\n", Mix_GetError());
    } else {
        Mix_PlayMusic(musique, -1); // jouer en boucle
    }

    while (1) {
        int choix = menu(renderer, font);
        if (choix == -1) break;
        if (choix == 0) instructions(renderer, font);
        if (choix == 1) jouer(renderer, font);
    }


    Mix_FreeMusic(musique);
    Mix_CloseAudio();
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return 0;
}
