#ifndef MENU_H
#define MENU_H

#include <SDL.h>
#include <SDL_ttf.h>

int menu(SDL_Renderer *r, TTF_Font *font);
int instructions(SDL_Renderer *r, TTF_Font *font);
void jouer(SDL_Renderer *renderer, TTF_Font *font);

#endif