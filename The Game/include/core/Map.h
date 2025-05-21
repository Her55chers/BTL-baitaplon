#pragma once
#include "core/Game.h"

class Map
{
public:
    Map();
    ~Map();

    void LoadMap(int arr[18][32]);
    void DrawMap();
private:
    SDL_Rect src, dst;

    SDL_Texture* ground;
    SDL_Texture* sky;
    SDL_Texture* grass;
    SDL_Texture* tree;

    int map[18][32];
};
