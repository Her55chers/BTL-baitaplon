#pragma once

#include "gameObject/Components.h"
#include "core/Vector2D.h"

class TransformComponent : public Component
{

public:

    Vector2D position;
    Vector2D velocity;

    int speed = 4;

    int width = 40;
    int height = 40;
    int scale = 1;

    TransformComponent()
    {
        position.Zero();
    }

    TransformComponent(float x, float y)
    {
        position.x = x;
        position.y = y;
    }

    TransformComponent(int scl)
    {
        position.Zero();
        scale = scl;
    }

    TransformComponent(float x, float y, int w, int h, int scl)
    {
        position.x = x;
        position.y = y;
        height = h;
        width = w;
        scale = scl;
    }

    void init() override
    {
        velocity.Zero();
    }

    void update() override
    {
        position.x += velocity.x * speed;
        position.y += velocity.y * speed;
    }
};

