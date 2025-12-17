#ifndef RENDER_H
#define RENDER_H

#include "config.h"
#include <SDL.h>
#include <SDL_ttf.h>

typedef struct { float x, y, vitesse; int taille; } Etoile;
typedef struct { float x, y, vitesse; int taille; SDL_Color couleur; } Planete;

void initialiserEtoiles(void);
void initialiserPlanetes(void);
void dessinerEtoiles(SDL_Renderer *r);
void dessinerPlanetes(SDL_Renderer *r);
void RemplirCercle(SDL_Renderer *r, int cx, int cy, int radius, SDL_Color color);
void drawPlanetGradient(SDL_Renderer *r, int cx, int cy, int radius, SDL_Color baseColor);
void afficherTexte(SDL_Renderer *renderer, TTF_Font *font, const char *texte, int x, int y);
void afficherTexteMultiligne(SDL_Renderer *renderer, TTF_Font *font, const char *texte, int x, int y);
SDL_Color colorForValue(int v);
int clique(int mx, int my, SDL_Rect r);
void dessinerGrille(SDL_Renderer *renderer, TTF_Font *font, int grille[4][4], int offsetX, int offsetY);

#endif // RENDER_H