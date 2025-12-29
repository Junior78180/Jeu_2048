#include "render.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

static Etoile etoiles[STARS];
static Planete planetes[PLANETES];
static Particule particules[MAX_PARTICULES];

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
        afficherTexte(renderer, font, ligne, x, y + i * ligneY + 4);
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

void creerExplosion(int x, int y, SDL_Color couleur) {
    /*
     * Crée une explosion de particules à la position (x, y) avec la couleur spécifiée
     */
    for (int i = 0; i < 20; i++) {
        int index = -1;
        for (int j = 0; j < MAX_PARTICULES; j++) {
            if (particules[j].vie <= 0) {
                index = j;
                break;
            }
        }
        if (index != -1) {
            particules[index].x = x;
            particules[index].y = y;
            float angle = (rand() % 360) * M_PI / 180.0f;
            // Vitesse réduite pour ralentir l'explosion
            float vitesse = 1.0f + (rand() % 100) / 100.0f;
            particules[index].vx = cos(angle) * vitesse;
            particules[index].vy = sin(angle) * vitesse;
            particules[index].vie = 60 + rand() % 40;
            particules[index].couleur = couleur;
        }
    }
}

void mettreAJourEtDessinerParticules(SDL_Renderer *r) {
    /*
     * Met à jour la position et la vie des particules, puis les dessine
     */
    for (int i = 0; i < MAX_PARTICULES; i++) {
        if (particules[i].vie > 0) {
            particules[i].x += particules[i].vx;
            particules[i].y += particules[i].vy;
            particules[i].vie--;

            SDL_SetRenderDrawColor(r, particules[i].couleur.r, particules[i].couleur.g, particules[i].couleur.b, (Uint8)(particules[i].vie * 10));
            // Particules agrandies (6x6)
            SDL_Rect rect = {(int)particules[i].x, (int)particules[i].y, 4, 4};
            SDL_RenderFillRect(r, &rect);
        }
    }
}
