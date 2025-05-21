#ifndef Game_h
#define Game_h


#include <string>
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <vector>

//class ColliderComponent;

class Game
{
public:
    Game();
    ~Game();
    //
    void gameInit(const char* tieude, int w, int h);
    bool running()
    {
        return gameRunning;
    }
    //
    void updateGame();
    void eventsGame();
    //
    void renderGame();
    void CleanUp();
    //
    static SDL_Renderer* renderer;
    static SDL_Event event;
    //static std::vector<ColliderComponent*> colliders;
    //
    void increaseScore();
    void resetScore();

private:
    //int count;
    bool gameRunning;
    SDL_Window* window;
};
#endif /* Game_h */
