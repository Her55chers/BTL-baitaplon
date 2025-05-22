#pragma once

#include "gameObject/Components.h"
#include "gameObject/TransformComponent.h"
#include <iostream>

class EnemyMovementComponent : public Component {
private:
    TransformComponent* transform;
    float speed;
    float deltaTime;

public:
    EnemyMovementComponent(float moveSpeed = 300.0f) : speed(moveSpeed), deltaTime(1.0f/60.0f) {}

    void init() override {
        transform = &entity->getComponent<TransformComponent>();
    }

    void update() override {
        // Di chuyển từ phải sang trái
        transform->position.x -= speed * deltaTime;

        // Kiểm tra nếu đã ra khỏi màn hình bên trái
        if (transform->position.x + transform->width * transform->scale < 0) {
            std::cout << "Enemy plane despawned at x: " << transform->position.x << std::endl;
            entity->destroy();
        }
    }
}; 