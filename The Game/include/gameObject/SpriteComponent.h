#pragma once

#include "gameObject/Components.h"
#include "core/Texture.h"
#include <SDL.h>

class SpriteComponent: public Component
{
private:
    TransformComponent* transform;
    SDL_Texture* texture;
    SDL_Rect srcRect, destRect;
    bool stretchToFit;

public:
    SpriteComponent() = default;
    SpriteComponent(const char* path) : stretchToFit(false)
    {
        setTex(path);
    }

    SpriteComponent(const char* path, bool stretch) : stretchToFit(stretch)
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

        srcRect.x = srcRect.y = 0;
        SDL_QueryTexture(texture, NULL, NULL, &srcRect.w, &srcRect.h);
        
        if (!stretchToFit) {
            destRect.w = transform->width * transform->scale;
            destRect.h = transform->height * transform->scale;
        } else {
            // Nếu stretch, sử dụng kích thước từ TransformComponent trực tiếp
            destRect.w = transform->width;
            destRect.h = transform->height;
        }
    }

    void update() override
    {
        destRect.x = static_cast<int>(transform->position.x);
        destRect.y = static_cast<int>(transform->position.y);
    }

    void draw() override
    {
        Texture::Draw(texture, srcRect, destRect);
    }
};
