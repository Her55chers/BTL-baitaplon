#pragma once
#include "Game.h"

class Texture
{
public:
    static SDL_Texture* loadTexture(const char* file);
    static void Draw(SDL_Texture* tex, SDL_Rect src, SDL_Rect dst);
};
