#pragma once

#include "gameObject/Components.h"
#include <iostream>

class HealthComponent : public Component {
private:
    int maxHealth;
    int currentHealth;

public:
    HealthComponent(int health) : maxHealth(health), currentHealth(health) {
        std::cout << "Entity initialized with " << health << " HP" << std::endl;
    }

    void takeDamage(int damage) {
        currentHealth -= damage;
        if (currentHealth < 0) currentHealth = 0;
        
        std::cout << "Entity took " << damage << " damage. Current HP: " << currentHealth << std::endl;
        
        if (currentHealth <= 0) {
            std::cout << "Entity destroyed due to HP reaching 0" << std::endl;
            entity->destroy();
        }
    }

    void heal(int amount) {
        currentHealth += amount;
        if (currentHealth > maxHealth) currentHealth = maxHealth;
        
        std::cout << "Entity healed for " << amount << ". Current HP: " << currentHealth << std::endl;
    }

    int getCurrentHealth() const { return currentHealth; }
    int getMaxHealth() const { return maxHealth; }
    
    bool isAlive() const { return currentHealth > 0; }
}; 