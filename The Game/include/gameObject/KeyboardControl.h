#pragma once

#include "core/Game.h"
#include "gameObject/ECS.h"
#include "gameObject/TransformComponent.h"
#include "gameObject/SpriteComponent.h"
#include "gameObject/ProjectileComponent.h"
#include "gameObject/CollisionComponent.h"

extern Manager manager;

class Keyboard : public Component
{
public:
    TransformComponent* transform;
    const Uint8* keystates = SDL_GetKeyboardState(NULL);

    void init() override
    {
        transform = &entity->getComponent<TransformComponent>();
    }

    /*void update() override
    {
        if(Game::event.type == SDL_KEYDOWN)
            {
                switch(Game::event.key.keysym.sym)
                {
                case SDLK_w:
                    transform->velocity.y = -1;
                    break;
                case SDLK_s:
                    transform->velocity.y = 1;
                    break;
                case SDLK_a:
                    transform->velocity.x = -1;
                    break;
                case SDLK_d:
                    transform->velocity.x = 1;
                    break;
                default:
                    break;
                }
            }

            if(Game::event.type == SDL_KEYUP)
            {
                switch(Game::event.key.keysym.sym)
                {
                case SDLK_w:
                    transform->velocity.y = 0;
                    break;
                case SDLK_s:
                    transform->velocity.y = 0;
                    break;
                case SDLK_a:
                    transform->velocity.x = 0;
                    break;
                case SDLK_d:
                    transform->velocity.x = 0;
                    break;
                default:
                    break;
                }
            }*/

    void update() override
    {
        transform->velocity.x = 0;
        transform->velocity.y = 0;
        if (keystates[SDL_SCANCODE_W] && transform->position.y >= -20)
        {
            transform->velocity.y = -1;
        }
        if (keystates[SDL_SCANCODE_A] && transform->position.x >= -20)
        {
            transform->velocity.x = -1;
        }
        if (keystates[SDL_SCANCODE_S] && transform->position.y <= 490)
        {
            transform->velocity.y = 1;
        }
        if (keystates[SDL_SCANCODE_D] && transform->position.x <= 1220)
        {
            transform->velocity.x = 1;
        }
    }
};
