#include "Texture.h"
#include "core/Game.h"
#include <iostream>

SDL_Texture* Texture::loadTexture(const char* file)
{
    std::cout << "Attempting to load texture: " << file << std::endl;
    
    SDL_Surface* tempSurface = IMG_Load(file);
    if (!tempSurface) {
        std::cout << "Failed to load image: " << file << std::endl;
        std::cout << "SDL_image Error: " << IMG_GetError() << std::endl;
        return nullptr;
    }

    SDL_Texture* tex = SDL_CreateTextureFromSurface(Game::renderer, tempSurface);
    if (!tex) {
        std::cout << "Failed to create texture from surface: " << file << std::endl;
        std::cout << "SDL Error: " << SDL_GetError() << std::endl;
    } else {
        std::cout << "Successfully loaded texture: " << file << std::endl;
    }

    SDL_FreeSurface(tempSurface);
    return tex;
}

void Texture::Draw(SDL_Texture* tex, SDL_Rect src, SDL_Rect dst)
{
    if (!tex) {
        std::cout << "Warning: Attempting to draw null texture!" << std::endl;
        return;
    }
    SDL_RenderCopy(Game::renderer, tex, &src, &dst);
}
