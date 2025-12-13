#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "config.h"
#include "render.h"
#include "menu.h"

int main() {
    srand((unsigned) time(NULL));
    initialiserEtoiles();
    initialiserPlanetes();

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
        return 1;
    }

    if (TTF_Init() < 0) {
        fprintf(stderr, "TTF_Init: %s\n", TTF_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("2048", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND); // Activer le blending

    // Charger la police
    TTF_Font *font = TTF_OpenFont("CHILLER.TTF", 40);
    if (!font) {
        printf("Erreur police %s\n", TTF_GetError());
    }

    // Initialiser l'audio
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("Erreur Mix_OpenAudio: %s\n", Mix_GetError());
    }

    // Charger et jouer la musique de fond
    Mix_Music *musique = Mix_LoadMUS("musics/Interstellar_Main_Theme_Extra_Extended_Soundtrack_by_Hans_Zimmer.mp3");
    if (musique) {
        Mix_PlayMusic(musique, -1);
    }

    while (1) {
        int choix = menu(renderer, font);
        if (choix == -1) break;
        if (choix == 0) instructions(renderer, font);
        if (choix == 1) jouer(renderer, font);
    }

    if (musique) {
        Mix_FreeMusic(musique);
    }
    Mix_CloseAudio();
    if (font) {
        TTF_CloseFont(font);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return 0;
}
