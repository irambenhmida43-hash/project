#include "game.h"
#include <time.h>

int main()
{
   srand(time(NULL));
    Game g;
    SDL_Event e;

    init(&g);
    loadMedia(&g);
    initPlatforms(&g);
    initObstacles(&g);

    while(g.running)
    {
        while(SDL_PollEvent(&e))
            handleEvents(&g,&e);

        update(&g);
        render(&g);

        SDL_Delay(16);
    }

    cleanup(&g);
    return 0;
}
