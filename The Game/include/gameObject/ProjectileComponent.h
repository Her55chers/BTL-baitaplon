#pragma once

#include "gameObject/ECS.h"
#include "gameObject/TransformComponent.h"
#include "core/Vector2D.h"
#include <iostream>

class ProjectileComponent : public Component {
private:
    TransformComponent* transform;
    Vector2D velocity;
    int range;
    float speed;
    Vector2D startPosition;
    float deltaTime;

public:
    ProjectileComponent(int rng, float xvel, float yvel) : range(rng), speed(800.0f), deltaTime(1.0f/60.0f)
    {
        // Đảm bảo luôn có hướng di chuyển
        velocity.x = (xvel != 0) ? xvel : 1.0f;
        velocity.y = yvel;
        
        // Chuẩn hóa vector vận tốc
        float length = sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
        if (length != 0) {
            velocity.x /= length;
            velocity.y /= length;
        }
    }

    void init() override {
        if (!entity->hasComponent<TransformComponent>()) {
            entity->addComponent<TransformComponent>();
        }
        transform = &entity->getComponent<TransformComponent>();
        startPosition = Vector2D(transform->position.x, transform->position.y);
        std::cout << "Projectile initialized at: " << startPosition.x << ", " << startPosition.y 
                 << " with velocity: " << velocity.x << ", " << velocity.y << std::endl;
    }

    void update() override {
        // Di chuyển projectile với deltaTime cố định
        transform->position.x += velocity.x * speed * deltaTime;
        transform->position.y += velocity.y * speed * deltaTime;

        // Tính kích thước thực của projectile
        int actualWidth = transform->width * transform->scale;
        int actualHeight = transform->height * transform->scale;

        // Kiểm tra nếu đạn hoàn toàn ra khỏi màn hình
        if (transform->position.x > 1280 + actualWidth || 
            transform->position.x < -actualWidth ||
            transform->position.y > 720 + actualHeight || 
            transform->position.y < -actualHeight)
        {
            std::cout << "Destroying projectile at: " << transform->position.x << ", " << transform->position.y << std::endl;
            entity->destroy();
        }
    }
};
