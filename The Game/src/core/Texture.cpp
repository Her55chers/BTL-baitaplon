#include "Texture.h"
#include "core/Game.h"

SDL_Texture* Texture::loadTexture(const char* file)
{
    SDL_Surface* tempSurface = IMG_Load(file);
    SDL_Texture* tex = SDL_CreateTextureFromSurface(Game::renderer, tempSurface);
    SDL_FreeSurface(tempSurface);

    return tex;
}

void Texture::Draw(SDL_Texture* tex, SDL_Rect src, SDL_Rect dst)
{
    SDL_RenderCopy(Game::renderer, tex, &src, &dst);
}
