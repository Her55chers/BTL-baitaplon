#pragma once

#include "gameObject/Ecs.h"
#include "gameObject/TransformComponent.h"
#include "gameObject/SpriteComponent.h"

#include <SDL.h>

class TileComponent : public Component
{
public:
    TransformComponent* transform;
    SpriteComponent* sprite;

    SDL_Rect tileRect;
    int tileID;
    const char* path;

    TileComponent() = default;

    TileComponent(int x, int y, int w, int h, int id)
    {
        tileRect.x = x;
        tileRect.y = y;
        tileRect.h = h;
        tileRect.w = w;
        tileID = id;

        switch(tileID)
        {
        case 2:
            path = "res/graphic/grass.png";
            break;
        default:
            break;
        }
    }

    void init() override
    {
        entity->addComponent<TransformComponent>((float)tileRect.x, (float)tileRect.y, (float)tileRect.w, (float)tileRect.h, 1);
        transform = &entity->getComponent<TransformComponent>();

        entity->addComponent<SpriteComponent>(path);
        sprite = &entity->getComponent<SpriteComponent>();
    }
};
