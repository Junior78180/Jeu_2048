#include "render.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

static Etoile etoiles[STARS];
static Planete planetes[PLANETES];

void initialiserEtoiles() {
    /*
     * Initialisation des étoiles avec des positions, vitesses et tailles aléatoires
     */
    for (int i = 0; i < STARS; i++) {
        etoiles[i].x = rand() % WIDTH;
        etoiles[i].y = rand() % HEIGHT;
        etoiles[i].vitesse = 0.5f + (rand() % 10) / 80.0f;
        etoiles[i].taille = 1 + rand() % 2;
    }
}

void initialiserPlanetes() {
    /*
     * Initialisation des planètes avec des positions, vitesses, tailles et couleurs aléatoires
     */
    for (int i = 0; i < PLANETES; i++) {
        planetes[i].x = rand() % WIDTH;
        planetes[i].y = rand() % HEIGHT;
        planetes[i].vitesse = 0.1f + (rand() % 150) / 200.0f;
        planetes[i].taille = 12 + rand() % 18;
        planetes[i].couleur.r = 120 + rand() % 5;
        planetes[i].couleur.g = 80 + rand() % 120;
        planetes[i].couleur.b = 150 + rand() % 150;
        planetes[i].couleur.a = 180;
    }
}

void RemplirCercle(SDL_Renderer *r, int cx, int cy, int radius, SDL_Color color) {
    /*
     * Remplit un cercle de centre (cx, cy) et de rayon 'radius' avec la couleur 'color'
     */
    SDL_SetRenderDrawColor(r, color.r, color.g, color.b, color.a);
    for (int dy = -radius; dy <= radius; dy++) {
        int dxMax = (int) sqrt(radius * radius - dy * dy);
        SDL_RenderDrawLine(r, cx - dxMax, cy + dy, cx + dxMax, cy + dy);
    }
}

void dessinerPlanetes(SDL_Renderer *r) {
    /*
     * Dessine les planètes avec un effet de dégradé et les fait descendre
     */
    for (int i = 0; i < PLANETES; i++) {
        int cx = (int) planetes[i].x;
        int cy = (int) planetes[i].y;
        int rad = planetes[i].taille;
        for (int k = 0; k < rad; k++) {
            float t = (float) k / rad;
            SDL_Color c = {
                (Uint8) (planetes[i].couleur.r * (1.0f - 0.5f * t)),
                (Uint8) (planetes[i].couleur.g * (1.0f - 0.5f * t)),
                (Uint8) (planetes[i].couleur.b * (1.0f - 0.5f * t)),
                20
            };
            RemplirCercle(r, cx, cy, rad - k, c);
        }
        planetes[i].y += planetes[i].vitesse;
        if (planetes[i].y > HEIGHT + rad) {
            planetes[i].x = rand() % WIDTH;
            planetes[i].y = -rad - 10;
        }
    }
}

void dessinerEtoiles(SDL_Renderer *r) {
    /*
     * Dessine les étoiles et les fait descendre
     */
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

SDL_Color colorForValue(int v) {
    /*
     * Définit la couleur en fonction de la valeur de la tuile
     */
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
    /*
     * Affiche le texte centré en (x, y)
     */
    if (!texte || !font) return;
    SDL_Color couleurTexte = {255, 255, 255, 255};
    SDL_Surface *surface = TTF_RenderText_Blended(font, texte, couleurTexte);
    if (!surface) return;
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect rect = {x - surface->w / 2, y - surface->h / 2, surface->w, surface->h};
    SDL_FreeSurface(surface);
    if (texture) {
        SDL_RenderCopy(renderer, texture, NULL, &rect);
        SDL_DestroyTexture(texture);
    }
}

void afficherTexteMultiligne(SDL_Renderer *renderer, TTF_Font *font, const char *texte, int x, int y) {
    /*
     * Affiche le texte multiligne en commençant en (x, y)
     */
    if (!texte) return;
    char *texteCopy = strdup(texte);
    if (!texteCopy) return;
    int ligneY = TTF_FontHeight(font);
    int i = 0;
    char *ligne = strtok(texteCopy, "\n");
    while (ligne) {
        afficherTexte(renderer, font, ligne, x, i * ligneY + 4);
        ligne = strtok(NULL, "\n");
        i++;
    }
    free(texteCopy);
}

int clique(int mx, int my, SDL_Rect r) {
    /*
     * Retourne 1 si le clic (mx, my) est dans le rectangle r, sinon 0
     */
    return mx >= r.x && mx <= r.x + r.w && my >= r.y && my <= r.y + r.h;
}

void dessinerGrille(SDL_Renderer *renderer, TTF_Font *font, int grille[4][4], int offsetX, int offsetY) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            SDL_Color c = colorForValue(grille[i][j]);
            SDL_Rect r = {j * CELL + 5 + offsetX, i * CELL + 5 + 100 + offsetY, CELL - 10, CELL - 10};
            SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
            SDL_RenderFillRect(renderer, &r);
            if (grille[i][j] != 0) {
                char txt[16];
                sprintf(txt, "%d", grille[i][j]);
                afficherTexte(renderer, font, txt, j * CELL + CELL / 2 + offsetX, i * CELL + CELL / 2 + 100 + offsetY);
            }
        }
    }
}
