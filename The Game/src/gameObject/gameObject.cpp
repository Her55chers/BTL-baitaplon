#include "gameObject/gameObject.h"
#include "core/Texture.h"
gameObject::gameObject(const char* textureSheet)
{
    objTexture = Texture::loadTexture(textureSheet);
}

void gameObject::Update()
{
    curX++;
    curY++;

    src.x = 0;
    src.y = 0;
    src.w = 1280;
    src.h = 720;

    dst.x = curX;
    dst.y = curY;
    dst.w = 40*2;
    dst.h = 40*2;

}

void gameObject::Render()
{
    SDL_RenderCopy(Game::renderer, objTexture, &src, &dst);
}
