#include "game.h"
#include <stdio.h>

void init(Game *g)
{
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();

    g->win=SDL_CreateWindow("GAME PRO",
        SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,W,H,0);

    g->ren=SDL_CreateRenderer(g->win,-1,SDL_RENDERER_ACCELERATED);

    g->running=1;
    g->mode=0;
    g->level=1;
    g->showGuide=0;

    g->p1.rect=(SDL_Rect){100,100,40,40};
    g->p2.rect=(SDL_Rect){200,100,40,40};

    g->cam1=(SDL_Rect){0,0,W,H};
    g->cam2=(SDL_Rect){0,0,W,H};

    g->font=TTF_OpenFont("arial.ttf",20);
  
}

void loadMedia(Game *g)
{
    g->menuBg=IMG_LoadTexture(g->ren,"bg.png");
    g->bg1=IMG_LoadTexture(g->ren,"Backback.png");
    g->bg2=IMG_LoadTexture(g->ren,"back.png");

    g->btnMono=IMG_LoadTexture(g->ren,"mono.png");
    g->btnMulti=IMG_LoadTexture(g->ren,"multi.png");

    g->guide=IMG_LoadTexture(g->ren,"guide.png");

    g->platFixe=IMG_LoadTexture(g->ren,"platformefixe.png");
    g->platMobile=IMG_LoadTexture(g->ren,"pmobile.png");

    g->destruct[0]=IMG_LoadTexture(g->ren,"floor1.png");
    g->destruct[1]=IMG_LoadTexture(g->ren,"floor2.png");
    g->destruct[2]=IMG_LoadTexture(g->ren,"floor3.png");
    // obstacle frames
    g->obs[0].frames[0]=IMG_LoadTexture(g->ren,"obs2.png");
    g->obs[0].frames[1]=IMG_LoadTexture(g->ren,"obs2.png");
    g->obs[0].frames[2]=IMG_LoadTexture(g->ren,"obs2.png");
    g->obs[0].frames[3]=IMG_LoadTexture(g->ren,"obs2.png");
    if(!g->obs[0].frames[0])
    printf("error obs image\n");
}

void initPlatforms(Game *g)
{
    g->plats[0]=(Platform){{300,400,200,40},0,0,1,FIXE,g->platFixe};

    g->plats[1]=(Platform){{600,300,150,40},2,2,1,MOBILE,g->platMobile};

    g->plats[2].rect=(SDL_Rect){1000,250,120,40};
    g->plats[2].type=DESTRUCTIBLE;
    g->plats[2].active=1;
    g->plats[2].frame=0;
    g->plats[2].timer=0;

    g->plats[2].frames[0]=g->destruct[0];
    g->plats[2].frames[1]=g->destruct[1];
    g->plats[2].frames[2]=g->destruct[2];
}
void initObstacles(Game *g)
{
    if(g->level == 1)
        g->nbObs = 5;   
    else
        g->nbObs = 8;   

    for(int i=0;i<g->nbObs;i++)
    {
        // position aléatoire
       g->obs[i].rect.x = rand() % (WORLD_W - 60);
       g->obs[i].rect.y = rand() % (WORLD_H - 60);
        g->obs[i].rect.w = 60;
        g->obs[i].rect.h = 60;

        //  speed
        if(g->level == 1)
            g->obs[i].speed = 0; 
        else
            g->obs[i].speed = 2 + rand()%4; 

       
        g->obs[i].currentFrame = 0;
        g->obs[i].frameDelay = 6 + rand()%5; 
        g->obs[i].frameTimer = 0;

        for(int j=0;j<4;j++)
            g->obs[i].frames[j] = g->obs[0].frames[j];
    }
}


void handleEvents(Game *g, SDL_Event *e)
{
    if(e->type==SDL_QUIT) g->running=0;

    if(g->mode==0 && e->type==SDL_MOUSEBUTTONDOWN)
    {
        int x=e->button.x,y=e->button.y;

        if(x>300&&x<500&&y>200&&y<260) g->mode=1;
        if(x>300&&x<500&&y>300&&y<360) g->mode=2;

        if(g->mode)
        {
            g->startTime1=SDL_GetTicks();
            g->startTime2=SDL_GetTicks();
        }
    }

    if(e->type==SDL_KEYDOWN && e->key.keysym.sym==SDLK_g)
        g->showGuide=!g->showGuide;
}

void update(Game *g)
{
    if(g->showGuide) return;

    const Uint8 *k=SDL_GetKeyboardState(NULL);

    // PLAYER1
    g->p1.vx=0; g->p1.vy=0;
    if(k[SDL_SCANCODE_RIGHT]) g->p1.vx=5;
    if(k[SDL_SCANCODE_LEFT]) g->p1.vx=-5;
    if(k[SDL_SCANCODE_UP]) g->p1.vy=-5;
    if(k[SDL_SCANCODE_DOWN]) g->p1.vy=5;

    g->p1.rect.x+=g->p1.vx;
    g->p1.rect.y+=g->p1.vy;

    // PLAYER2
    if(g->mode==2)
    {
        g->p2.vx=0; g->p2.vy=0;
        if(k[SDL_SCANCODE_D]) g->p2.vx=5;
        if(k[SDL_SCANCODE_Q]) g->p2.vx=-5;
        if(k[SDL_SCANCODE_Z]) g->p2.vy=-5;
        if(k[SDL_SCANCODE_S]) g->p2.vy=5;

        g->p2.rect.x+=g->p2.vx;
        g->p2.rect.y+=g->p2.vy;
    }

    // PLATFORMS
    for(int i=0;i<3;i++)
    {
        Platform *p=&g->plats[i];
        if(!p->active) continue;

        if(p->type==MOBILE)
        {
            p->rect.x+=p->vx;
            p->rect.y+=p->vy;

            if(p->rect.x<400||p->rect.x>1200) p->vx*=-1;
            if(p->rect.y<200||p->rect.y>500) p->vy*=-1;
        }
    }

    // CAMERA
    g->cam1.x=g->p1.rect.x-W/2;
    g->cam1.y=g->p1.rect.y-H/2;

    g->cam2.x=g->p2.rect.x-W/2;
    g->cam2.y=g->p2.rect.y-H/2;

    if(g->cam1.x<0) g->cam1.x=0;
    if(g->cam1.y<0) g->cam1.y=0;
    if(g->cam1.x>WORLD_W-W) g->cam1.x=WORLD_W-W;
    if(g->cam1.y>WORLD_H-H) g->cam1.y=WORLD_H-H;

    if(g->cam2.x<0) g->cam2.x=0;
    if(g->cam2.y<0) g->cam2.y=0;
    if(g->cam2.x>WORLD_W-W) g->cam2.x=WORLD_W-W;
    if(g->cam2.y>WORLD_H-H) g->cam2.y=WORLD_H-H;

    // LEVEL CHANGE
    if(g->p1.rect.x>1800)
    {
        g->level=2;

        g->p1.rect.x=100;
        g->p1.rect.y=100;

        g->p2.rect.x=200;
        g->p2.rect.y=100;
        initObstacles(g);
    }
    for(int i=0;i<g->nbObs;i++)
{
    // mouvement level 2
    if(g->level==2)
    {
        g->obs[i].rect.x+=g->obs[i].speed;

        if(g->obs[i].rect.x<300 || g->obs[i].rect.x>1700)
            g->obs[i].speed*=-1;
    }

    // animation
    g->obs[i].frameTimer++;

    if(g->obs[i].frameTimer>=g->obs[i].frameDelay)
    {
        g->obs[i].frameTimer=0;
        g->obs[i].currentFrame++;

        if(g->obs[i].currentFrame>=4)
            g->obs[i].currentFrame=0;
    }

    // collision P1
    if(SDL_HasIntersection(&g->p1.rect,&g->obs[i].rect))
    {
        g->p1.rect.x=100;
        g->p1.rect.y=100;
    }

    // collision P2
    if(g->mode==2 && SDL_HasIntersection(&g->p2.rect,&g->obs[i].rect))
    {
        g->p2.rect.x=200;
        g->p2.rect.y=100;
    }
}
}

void drawScene(Game *g, Player *p, SDL_Rect cam)
{
    SDL_Texture *bg=(g->level==1)?g->bg1:g->bg2;

    SDL_Rect dst={-cam.x,-cam.y,WORLD_W,WORLD_H};
    SDL_RenderCopy(g->ren,bg,NULL,&dst);

    for(int i=0;i<3;i++)
    {
        Platform *pl=&g->plats[i];
        if(!pl->active) continue;

        SDL_Rect d={pl->rect.x-cam.x,pl->rect.y-cam.y,pl->rect.w,pl->rect.h};

        if(pl->type==DESTRUCTIBLE)
            SDL_RenderCopy(g->ren,pl->frames[pl->frame],NULL,&d);
        else
            SDL_RenderCopy(g->ren,pl->tex,NULL,&d);
    }

    SDL_Rect pr={p->rect.x-cam.x,p->rect.y-cam.y,p->rect.w,p->rect.h};
    SDL_RenderFillRect(g->ren,&pr);
    for(int i=0;i<g->nbObs;i++)
{
    SDL_Rect d={
        g->obs[i].rect.x-cam.x,
        g->obs[i].rect.y-cam.y,
        g->obs[i].rect.w,
        g->obs[i].rect.h
    };

  if(g->obs[i].frames[g->obs[i].currentFrame])
{
    SDL_RenderCopy(g->ren,
        g->obs[i].frames[g->obs[i].currentFrame],
        NULL,&d);
}
}
}

void render(Game *g)
{
    SDL_RenderClear(g->ren);

    if(g->mode==0)
    {
        SDL_RenderCopy(g->ren,g->menuBg,NULL,NULL);

        SDL_Rect r1={300,200,200,60};
        SDL_Rect r2={300,300,200,60};

        SDL_RenderCopy(g->ren,g->btnMono,NULL,&r1);
        SDL_RenderCopy(g->ren,g->btnMulti,NULL,&r2);
    }
    else if(g->mode==1)
    {
        drawScene(g,&g->p1,g->cam1);
    }
    else
    {
        SDL_Rect left={0,0,W/2,H};
        SDL_Rect right={W/2,0,W/2,H};

        SDL_RenderSetViewport(g->ren,&left);
        drawScene(g,&g->p1,g->cam1);

        SDL_RenderSetViewport(g->ren,&right);
        drawScene(g,&g->p2,g->cam2);

        SDL_RenderSetViewport(g->ren,NULL);
    }

    // GUIDE
    if(g->showGuide)
    {
        SDL_Rect r={200,150,400,300};
        SDL_RenderCopy(g->ren,g->guide,NULL,&r);
    }
    else if(g->mode!=0)
    {
        SDL_Color c={255,255,255};

        char t1[50];
        sprintf(t1,"P1: %d",(SDL_GetTicks()-g->startTime1)/1000);

        SDL_Surface *s1=TTF_RenderText_Solid(g->font,t1,c);
        SDL_Texture *tx1=SDL_CreateTextureFromSurface(g->ren,s1);

        SDL_Rect r1={10,10,s1->w,s1->h};
        SDL_RenderCopy(g->ren,tx1,NULL,&r1);

        SDL_FreeSurface(s1);
        SDL_DestroyTexture(tx1);

        if(g->mode==2)
        {
            char t2[50];
            sprintf(t2,"P2: %d",(SDL_GetTicks()-g->startTime2)/1000);

            SDL_Surface *s2=TTF_RenderText_Solid(g->font,t2,c);
            SDL_Texture *tx2=SDL_CreateTextureFromSurface(g->ren,s2);

            SDL_Rect r2={650,10,s2->w,s2->h};
            SDL_RenderCopy(g->ren,tx2,NULL,&r2);

            SDL_FreeSurface(s2);
            SDL_DestroyTexture(tx2);
        }
    }

    SDL_RenderPresent(g->ren);
}

void cleanup(Game *g)
{
    SDL_DestroyRenderer(g->ren);
    SDL_DestroyWindow(g->win);
    SDL_Quit();
}
