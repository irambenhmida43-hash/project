#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#define W 800
#define H 600
#define WORLD_W 2000
#define WORLD_H 1000

typedef enum { FIXE, MOBILE, DESTRUCTIBLE } TypePlat;

typedef struct {
    SDL_Rect rect;
    int vx, vy;
    int active;
    TypePlat type;
    SDL_Texture *tex;
    SDL_Texture *frames[3];
    int frame, timer;
} Platform;

typedef struct {
    SDL_Rect rect;
    int vx, vy;
} Player;
typedef struct
{
    SDL_Rect rect;

    SDL_Texture *frames[4];
    int currentFrame;
    int frameDelay;
    int frameTimer;

    int speed;
} Obstacle;

typedef struct {
    SDL_Window *win;
    SDL_Renderer *ren;

    SDL_Texture *menuBg,*bg1,*bg2;
    SDL_Texture *btnMono,*btnMulti;
    SDL_Texture *guide;

    SDL_Texture *platFixe,*platMobile,*destruct[3];

    Player p1,p2;
    SDL_Rect cam1,cam2;

    Platform plats[3];
    Obstacle obs[10];
    int nbObs;

    int mode, level, running, showGuide;

    Uint32 startTime1,startTime2;

    TTF_Font *font;

} Game;

void init(Game*);
void loadMedia(Game*);
void initPlatforms(Game*);
void initObstacles(Game *);
void handleEvents(Game*,SDL_Event*);
void update(Game*);
void render(Game*);
void cleanup(Game*);

#endif
