#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define WIDTH 400  // largeur de la fenêtre
#define HEIGHT 400   // hauteur de la fenêtre
#define CELL  (WIDTH/4)  // taille d'une cellule

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
        grille[x][y] = (rand() % 5 == 0) ? 4 : 2;
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
                temp[j + 1] = 0;
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
    copierGrille(grille, temp); deplacerGauche(temp); if (!grillesIdentiques(grille,temp)) return 1;
    copierGrille(grille, temp); deplacerDroite(temp); if (!grillesIdentiques(grille,temp)) return 1;
    copierGrille(grille, temp); deplacerHaut(temp); if (!grillesIdentiques(grille,temp)) return 1;
    copierGrille(grille, temp); deplacerBas(temp); if (!grillesIdentiques(grille,temp)) return 1;
    return 0;
}

SDL_Color colorForValue(int v) {
    if (v==2) return (SDL_Color){238,228,218,255};
    if (v==4) return (SDL_Color){237,224,200,255};
    if (v==8) return (SDL_Color){242,177,121,255};
    if (v==16) return (SDL_Color){245,149,99,255};
    if (v==32) return (SDL_Color){246,124,95,255};
    if (v==64) return (SDL_Color){246,94,59,255};
    if (v>64) return (SDL_Color){237,207,114,255};
    return (SDL_Color){205,193,180,255};
}

void afficherTexte(SDL_Renderer* renderer, TTF_Font* font, int valeur, int x, int y) {
    if (valeur == 0) return;
    char texte[8];
    sprintf(texte, "%d", valeur);

    SDL_Color noir = {50, 50, 50, 255};
    SDL_Surface* surface = TTF_RenderText_Blended(font, texte, noir);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect rect = {x + CELL/2 - surface->w/2, y + CELL/2 - surface->h/2, surface->w, surface->h};
    SDL_FreeSurface(surface);

    SDL_RenderCopy(renderer, texture, NULL, &rect);
    SDL_DestroyTexture(texture);
}


int main() {
    srand((unsigned int)time(NULL));
    int grille[4][4]={0}, ancienne[4][4];

    if(SDL_Init(SDL_INIT_VIDEO)!=0){printf("Erreur SDL: %s\n",SDL_GetError());return 1;}
    if(TTF_Init()!=0){printf("Erreur TTF: %s\n",TTF_GetError());return 1;}

    SDL_Window* window = SDL_CreateWindow("2048", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    TTF_Font* font = TTF_OpenFont("CHILLER.TTF", 40);
    if(!font){printf("Erreur ouverture police: %s\n",TTF_GetError());return 1;}

    ajouterTuile(grille); ajouterTuile(grille);

    int running = 1; SDL_Event e;
    while(running){
        while(SDL_PollEvent(&e)){
            if(e.type==SDL_QUIT) running=0;
            else if(e.type==SDL_KEYDOWN){
                copierGrille(grille, ancienne);
                switch(e.key.keysym.sym){
                    case SDLK_z: deplacerHaut(grille); break;
                    case SDLK_q: deplacerGauche(grille); break;
                    case SDLK_s: deplacerBas(grille); break;
                    case SDLK_d: deplacerDroite(grille); break;
                    case SDLK_x: running=0; break;
                        default: break;
                }
                if(!grillesIdentiques(grille,ancienne)) ajouterTuile(grille);
            }
        }

        SDL_SetRenderDrawColor(renderer, 187,173,160,255);
        SDL_RenderClear(renderer);

        for(int i=0;i<4;i++){
            for(int j=0;j<4;j++){
                SDL_Color c=colorForValue(grille[i][j]);
                SDL_Rect rect={j*CELL+5,i*CELL+5,CELL-10,CELL-10};
                SDL_SetRenderDrawColor(renderer,c.r,c.g,c.b,c.a);
                SDL_RenderFillRect(renderer,&rect);

                afficherTexte(renderer, font, grille[i][j], j*CELL, i*CELL);
            }
        }

        SDL_RenderPresent(renderer);

        if(!mouvementPossible(grille)){
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,"Fin de partie","Aucun mouvement possible !",window);
            running=0;
        }

        SDL_Delay(16);
    }

    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
