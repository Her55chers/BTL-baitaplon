#pragma once

#include "gameObject/Components.h"
#include "core/Texture.h"
#include <SDL.h>

class SpriteComponent: public Component
{
private:
    TransformComponent* transform;
    SDL_Texture* texture;
    SDL_Rect src, dst;

public:
    SpriteComponent() = default;
    SpriteComponent(const char* path)
    {
        setTex(path);
    }

    ~SpriteComponent()
    {
        SDL_DestroyTexture(texture);
    }

    void setTex(const char* path)
    {
        texture = Texture::loadTexture(path);
    }

    void init() override
    {
        transform = &entity->getComponent<TransformComponent>();

        src.x = src.y;
        src.w = transform->width;
        src.h = transform->height;

        dst.w = dst.h = 40;
    }

    void update() override
    {
        dst.x = static_cast<int>(transform->position.x);
        dst.y = static_cast<int>(transform->position.y);
        dst.w = transform->width * transform->scale;
        dst.h = transform->height * transform->scale;
    }

    void draw() override
    {
        Texture::Draw(texture, src, dst);
    }
};
