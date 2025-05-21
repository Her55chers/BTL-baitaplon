#pragma once
#include "Game.h"
class gameObject
{
public:
    gameObject(const char* textureSheet);
    ~gameObject();

    void Update();
    void Render();
private:
    int curX, curY;

    SDL_Texture* objTexture;
    SDL_Rect src, dst;
};
