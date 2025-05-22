#pragma once

#include <string>
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <vector>

#include "gameObject/AssetsManager.h"

//class ColliderComponent;

class Game
{
public:
    Game();
    ~Game();
    //
    void gameInit(const char* tieude, int w, int h);
    bool running() const { return gameRunning; }
    //
    void updateGame();
    void eventsGame();
    //
    void renderGame();
    void CleanUp();
    //
    static SDL_Renderer* renderer;
    static SDL_Event event;
  //  static AssetManager* assets;
    //static std::vector<ColliderComponent*> colliders;
    //
    void increaseScore(int amount = 100);
    int getCurrentScore() const;
    void resetScore();
    void renderScore();
    int getCurrentLevel() const;
    void increaseLevel();
    void resetLevel();

private:
    //int count;
    bool gameRunning;
    bool isGameOver;
    Entity* gameOverScreen;
    SDL_Window* window;
    
    // Score variables
    int currentScore = 0;
    int currentLevel = 1;
    TTF_Font* scoreFont = nullptr;
    SDL_Color scoreColor = {255, 255, 255, 255}; // White color
    
    // Timing variables
    Uint32 lastProjectileTime = 0;
    Uint32 lastEnemySpawnTime = 0;
    static const Uint32 PROJECTILE_INTERVAL = 400; // 400ms = 0.4s between shots
    static const Uint32 ENEMY_SPAWN_INTERVAL = 2000; // 2000ms = 2s between enemy spawns

    void spawnEnemyPlane();
    void spawnEnemyTank();
    void spawnEnemyBullet(float startX, float startY);
    void spawnApple();
    void spawnEnemyBoss();
    void spawnEnemyDrone(float centerX, float centerY, float angleOffset);
};
