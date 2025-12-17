#include "menu.h"
#include "render.h"
#include "game.h"
#include <stdio.h>
#include <string.h>

int menu(SDL_Renderer *r, TTF_Font *font) {
    /*
     * Affichage graphique du menu principal de l'application
     * Permet de choisir différentes options comme jouer, changer la difficulté, quitter ou afficher les instructions
     * Retourne un entier correspondant au choix de l'utilisateur :
     *  1 : Jouer
     *  changement de difficulté (0: facile, 1: difficile) pour la variable globale 'difficulty'
     *  0 : Instructions
     * -1 : Quitter
     */
    int running = 1;
    SDL_Event e;

    SDL_Rect btnInstructions = {175, 350, 50, 50};
    SDL_Rect btnPlay = {100, 100, 200, 60};
    SDL_Rect btnDiff = {90, 180, 220, 60};
    SDL_Rect btnQuit = {100, 260, 200, 60};

    while (running) {
        while (SDL_PollEvent(&e)) {
            /*
             * Gestion des événements SDL
             */
            if (e.type == SDL_QUIT) return -1;
            if (e.type == SDL_MOUSEBUTTONDOWN) {
                int mx = e.button.x, my = e.button.y;
                if (clique(mx, my, btnPlay)) return 1;
                if (clique(mx, my, btnDiff)) { difficulty = (difficulty == 0 ? 1 : 0); }
                if (clique(mx, my, btnQuit)) return -1;
                if (clique(mx, my, btnInstructions)) return 0;
            }
        }

        dessinerEtoiles(r);
        dessinerPlanetes(r);

        SDL_SetRenderDrawColor(r, 50, 50, 50, 150);
        SDL_RenderFillRect(r, &btnInstructions);
        SDL_RenderFillRect(r, &btnPlay);
        SDL_RenderFillRect(r, &btnDiff);
        SDL_RenderFillRect(r, &btnQuit);

        afficherTexte(r, font, "2048 en C", 200, 50);
        afficherTexte(r, font, "Jouer", 200, 130);
        if (difficulty == 0) afficherTexte(r, font, "Difficulte: Facile", 200, 210);
        else afficherTexte(r, font, "Difficulte: Difficile", 200, 210);
        afficherTexte(r, font, "Quitter", 200, 290);
        afficherTexte(r, font, "?", 200, 375);

        SDL_RenderPresent(r);
        SDL_Delay(16);
    }
    return -1;
}

int instructions(SDL_Renderer *r, TTF_Font *font) {
    /*
     * Affichage graphique des instructions du jeu
     * Permet à l'utilisateur de lire les instructions et de revenir au menu principal
     */
    int running = 1;
    SDL_Event e;
    SDL_Rect fondTexteInstruction = {20, 120, 360, 280};
    SDL_Rect back = {175, 425, 50, 50};

    TTF_Font *small = TTF_OpenFont("CHILLER.ttf", 26);
    if (!small) return -1;

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
            if (e.type == SDL_QUIT) { TTF_CloseFont(small); return -1; }
            if (e.type == SDL_MOUSEBUTTONDOWN) {
                int mx = e.button.x, my = e.button.y;
                if (clique(mx, my, back)) { TTF_CloseFont(small); return -1; }
            }
        }

        dessinerEtoiles(r);
        dessinerPlanetes(r);
        afficherTexte(r, font, "2048 en C", 200, 50);
        afficherTexte(r, font, "Instructions :", 200, 90);
        SDL_SetRenderDrawColor(r, 50, 50, 50, 50);
        SDL_RenderFillRect(r, &fondTexteInstruction);
        afficherTexteMultiligne(r, small, texte, 200, 130);

        SDL_SetRenderDrawColor(r, 50, 50, 50, 150);
        SDL_RenderFillRect(r, &back);
        afficherTexte(r, font, "X", 200, 450);

        SDL_RenderPresent(r);
        SDL_Delay(16);
    }
    TTF_CloseFont(small);
    return -1;
}

void jouer(SDL_Renderer *renderer, TTF_Font *font) {
    /*
     * Affichage graphique de la partie en cours
     */
    int grille[4][4] = {0}, old[4][4];
    chargerHighscore();
    score = 0;
    int lastScore = highscore;
    ajouterTuile(grille);
    ajouterTuile(grille);

    int running = 1;
    SDL_Event e;
    char txtScore[32];
    char txtHighscore[32];

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                if (highscore != lastScore) sauvegarderHighscore();
                return;
            }
            if (e.type == SDL_KEYDOWN) {
                copierGrille(grille, old);
                switch (e.key.keysym.sym) {
                    case SDLK_z: deplacerHaut(grille, 1); break;
                    case SDLK_q: deplacerGauche(grille, 1); break;
                    case SDLK_s: deplacerBas(grille, 1); break;
                    case SDLK_d: deplacerDroite(grille, 1); break;
                    case SDLK_x:
                        if (highscore != lastScore) sauvegarderHighscore();
                        return;
                }
                if (memcmp(grille, old, sizeof grille) != 0) ajouterTuile(grille); //
            }
        }

        sprintf(txtScore, "Score: %d", score);
        sprintf(txtHighscore, "Highscore: %d", highscore);
        if (highscore != lastScore) { sauvegarderHighscore(); lastScore = highscore; }

        dessinerEtoiles(renderer);
        dessinerPlanetes(renderer);

        afficherTexte(renderer, font, txtScore, WIDTH / 2, 30);
        afficherTexte(renderer, font, txtHighscore, WIDTH / 2, 80);

        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                SDL_Color c = colorForValue(grille[i][j]);
                SDL_Rect r = {j * CELL + 5, i * CELL + 5 + 100, CELL - 10, CELL - 10};
                SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
                SDL_RenderFillRect(renderer, &r);
                if (grille[i][j] != 0) {
                    char txt[16];
                    sprintf(txt, "%d", grille[i][j]);
                    afficherTexte(renderer, font, txt, j * CELL + CELL / 2, i * CELL + CELL / 2 + 100);
                }
            }
        }

        SDL_RenderPresent(renderer);

        if (!mouvementPossible(grille)) {
            if (highscore != lastScore) sauvegarderHighscore();
            int choix = gameOver(renderer, font);
            if (choix == 1) { // Réessayer
                jouer(renderer, font);
            }
            return; // Retour au menu
        }

        SDL_Delay(16);
    }
}

int gameOver(SDL_Renderer *r, TTF_Font *font) {
    /*
     * Affichage graphique de l'écran de game over
     * Permet de choisir de réessayer ou de revenir au menu principal
     * Retourne 1 pour réessayer, 0 pour le menu
     */
    int running = 1;
    SDL_Event e;

    SDL_Rect btnRetry = {100, 200, 200, 60};
    SDL_Rect btnMenu = {100, 280, 200, 60};

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) return 0;
            if (e.type == SDL_MOUSEBUTTONDOWN) {
                int mx = e.button.x, my = e.button.y;
                if (clique(mx, my, btnRetry)) return 1;
                if (clique(mx, my, btnMenu)) return 0;
            }
        }

        dessinerEtoiles(r);
        dessinerPlanetes(r);

        afficherTexte(r, font, "Game Over", 200, 100);

        SDL_SetRenderDrawColor(r, 50, 50, 50, 150);
        SDL_RenderFillRect(r, &btnRetry);
        SDL_RenderFillRect(r, &btnMenu);

        afficherTexte(r, font, "Reessayer", 200, 260);
        afficherTexte(r, font, "Menu", 200, 340);

        SDL_RenderPresent(r);
        SDL_Delay(16);
    }
    return 0;
}
