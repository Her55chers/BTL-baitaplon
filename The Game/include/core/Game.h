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
    bool running()
    {
        return gameRunning;
    }
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
    int getCurrentScore() const { return currentScore; }
    void resetScore() { currentScore = 0; }
    void renderScore();

private:
    //int count;
    bool gameRunning;
    SDL_Window* window;
    
    // Score variables
    int currentScore = 0;
    TTF_Font* scoreFont = nullptr;
    SDL_Color scoreColor = {255, 255, 255, 255}; // White color
    
    // Timing variables
    Uint32 lastProjectileTime = 0;
    Uint32 lastEnemySpawnTime = 0;
    static const Uint32 PROJECTILE_INTERVAL = 400; // 400ms = 0.4s between shots
    static const Uint32 ENEMY_SPAWN_INTERVAL = 2000; // 2000ms = 2s between enemy spawns

    void spawnEnemyPlane();
};
