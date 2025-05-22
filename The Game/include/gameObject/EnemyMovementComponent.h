#pragma once

#include "gameObject/Components.h"
#include "gameObject/TransformComponent.h"
#include "core/Vector2D.h"
#include <iostream>

class EnemyMovementComponent : public Component {
private:
    TransformComponent* transform;
    float speed;
    float deltaTime;
    const float BOSS_FIXED_X = 1180.0f;

public:
    Vector2D velocity;
    bool isBoss;

    EnemyMovementComponent(float moveSpeed = 300.0f, bool boss = false) 
        : speed(moveSpeed), deltaTime(1.0f/60.0f), isBoss(boss) {
        velocity.x = -1.0f;  // Di chuyển sang trái cho enemy thường
        velocity.y = boss ? 1.0f : 0.0f;  // Chỉ boss mới di chuyển lên xuống
    }

    void setSpeed(float newSpeed) {
        speed = newSpeed;
    }

    float getSpeed() const {
        return speed;
    }

    void init() override {
        transform = &entity->getComponent<TransformComponent>();
        if (isBoss) {
            transform->position.x = BOSS_FIXED_X;
        }
    }

    void update() override {
        if (isBoss) {
            // Boss chỉ di chuyển lên xuống, giữ nguyên vị trí X
            transform->position.x = BOSS_FIXED_X;
            transform->position.y += velocity.y * speed * deltaTime;
        } else {
            // Enemy thường chỉ di chuyển sang trái
            transform->position.x += velocity.x * speed * deltaTime;
        }

        // Kiểm tra nếu đã ra khỏi màn hình bên trái (chỉ áp dụng cho enemy thường)
        if (!isBoss && transform->position.x + transform->width * transform->scale < 0) {
            std::cout << "Enemy plane despawned at x: " << transform->position.x << std::endl;
            entity->destroy();
        }
    }
}; 