#include "Game.h"


Game* game = nullptr;

int main(int argc, char** argv)
{
    game = new Game();

    game->gameInit("The Game", 1280, 720);

    const int FPS  = 60;
    const int frameDelay = 1000/FPS;

    Uint32 frameStart;
    int frameTime;




    while(game->running())
    {
        frameTime = SDL_GetTicks() - frameStart;

        if(frameDelay > frameTime)
        {
            SDL_Delay(frameDelay - frameTime);
        }
    //
        game->eventsGame();
        game->updateGame();
        game->renderGame();

    }

    game->CleanUp();

return 0;
}
