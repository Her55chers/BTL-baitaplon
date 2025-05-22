#pragma once

#include "gameObject/Components.h"
#include "gameObject/TransformComponent.h"
#include "core/Vector2D.h"
#include <iostream>
#include <cmath>

class DroneMovementComponent : public Component {
private:
    TransformComponent* transform;
    float radius;         // Bán kính quay
    float angle;         // Góc hiện tại
    float angleSpeed;    // Tốc độ quay (radian/giây)
    float centerX;       // Tọa độ tâm X
    float centerY;       // Tọa độ tâm Y
    float deltaTime;

public:
    DroneMovementComponent(float r = 80.0f, float startAngle = 0.0f, float speed = 2.0f) 
        : radius(r), angle(startAngle), angleSpeed(speed), deltaTime(1.0f/60.0f) {}

    void init() override {
        transform = &entity->getComponent<TransformComponent>();
    }

    void setCenterPoint(float x, float y) {
        centerX = x;
        centerY = y;
    }

    void update() override {
        // Cập nhật góc
        angle += angleSpeed * deltaTime;
        if (angle > 2 * M_PI) {
            angle -= 2 * M_PI;
        }

        // Tính toán vị trí mới dựa trên góc và bán kính
        transform->position.x = centerX + radius * cos(angle);
        transform->position.y = centerY + radius * sin(angle);
    }
}; 