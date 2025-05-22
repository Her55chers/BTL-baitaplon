#include "core/Game.h"
#include "core/Texture.h"
#include "core/Map.h"
#include "core/Vector2D.h"
#include "core/Collision.h"

#include "gameObject/Components.h"
#include "gameObject/ProjectileComponent.h"

Manager manager;
Map* map;

SDL_Renderer* Game::renderer = nullptr;
SDL_Event Game::event;

//AssetManager* Game::assets = new AssetManager(&manager);

auto& player(manager.addEntity());


const Uint32 FIXED_TIMESTEP = 16; // 16ms = ~60fps
const Uint32 PROJECTILE_INTERVAL = 400; // 400ms = 0.4s between shots
const Uint32 ENEMY_SPAWN_INTERVAL = 2000; // 2000ms = 2s between enemy spawns

enum groupName : std::size_t
{
    groupPlayers,
    groupEnemies,
    groupMap
};

Game::Game()
{
    lastProjectileTime = SDL_GetTicks();
    lastEnemySpawnTime = SDL_GetTicks();
}

Game::~Game()
{
}


void Game::gameInit(const char* tieude, int w, int h)
{
    srand(static_cast<unsigned int>(time(0)));

    if(SDL_Init(SDL_INIT_EVERYTHING) == 0){
        std::cout << "Khoi tao thanh cong!" << std::endl;

        window = SDL_CreateWindow(tieude, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_SHOWN);
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

        // Khởi tạo TTF
        if(TTF_Init() == -1) {
            std::cout << "TTF init failed: " << TTF_GetError() << std::endl;
            gameRunning = false;
            return;
        }

        // Load font
                scoreFont = TTF_OpenFont("res/fonts/Roboto-Regular.ttf", 24);        if(!scoreFont) {            std::cout << "Font loading failed: " << TTF_GetError() << std::endl;            gameRunning = false;            return;        }

        gameRunning = true;
    }
    else{
        std::cout << "Khoi tao khong thanh cong!" << std::endl;
        gameRunning = false;
    }

    map = new Map();

    // Khởi tạo player với 100 HP
    player.addComponent<TransformComponent>(0.0f, 0.0f, 40, 40, 2);
    player.addComponent<SpriteComponent>("res/graphic/main_char.png");
    player.addComponent<Keyboard>();
    player.addComponent<ColliderComponent>("player");
    player.addComponent<HealthComponent>(100);
    player.addGroup(groupPlayers);

    resetScore(); // Reset score khi bắt đầu game
}

void Game::spawnEnemyPlane()
{
    // Tính toán vị trí spawn trong 3/5 phần trên của màn hình
    float spawnX = 1280.0f; // Bên phải màn hình
    const int SCREEN_HEIGHT = 720;
    const int MAX_SPAWN_HEIGHT = (SCREEN_HEIGHT * 3) / 5; // 3/5 của chiều cao màn hình = 432px
    float spawnY = rand() % MAX_SPAWN_HEIGHT; // Vị trí Y ngẫu nhiên từ 0 đến 432px

    auto& enemy_plane(manager.addEntity());
    enemy_plane.addComponent<TransformComponent>(spawnX, spawnY, 40, 40, 1);
    enemy_plane.addComponent<SpriteComponent>("res/graphic/enemy_plane.png");
    enemy_plane.addComponent<ColliderComponent>("enemy_plane");
    enemy_plane.addComponent<HealthComponent>(30);
    enemy_plane.addComponent<EnemyMovementComponent>(300.0f);
    enemy_plane.addGroup(groupEnemies);
    
    std::cout << "Enemy plane spawned at (" << spawnX << ", " << spawnY << ") with 30 HP" << std::endl;
}

auto& players(manager.getGroup(groupPlayers));

void Game::increaseScore(int amount)
{
    currentScore += amount;
    std::cout << "Score increased by " << amount << ". Current score: " << currentScore << std::endl;
}

void Game::renderScore()
{
    std::string scoreText = "Your Score: " + std::to_string(currentScore);
    SDL_Surface* surface = TTF_RenderText_Solid(scoreFont, scoreText.c_str(), scoreColor);
    if (surface == nullptr) {
        std::cout << "Unable to render text surface: " << TTF_GetError() << std::endl;
        return;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (texture == nullptr) {
        std::cout << "Unable to create texture from rendered text: " << SDL_GetError() << std::endl;
        return;
    }

    SDL_Rect destRect = { 10, 10, surface->w, surface->h }; // Vị trí góc trái trên
    SDL_RenderCopy(renderer, texture, NULL, &destRect);
    SDL_DestroyTexture(texture);
}

void Game::renderGame()
{
    SDL_RenderClear(renderer);
    
    // Vẽ map
    map->DrawMap();

    // Vẽ tất cả các entities theo thứ tự group
    for(auto& p : manager.getGroup(groupMap))
    {
        p->draw();
    }
    
    for(auto& p : manager.getGroup(groupPlayers))
    {
        p->draw();
    }

    for(auto& e : manager.getGroup(groupEnemies))
    {
        e->draw();
    }

    // Render score
    renderScore();

    SDL_RenderPresent(renderer);
}

void Game::updateGame()
{
    // Cập nhật trạng thái game
    Vector2D playerPos = player.getComponent<TransformComponent>().position;
    auto& transform = player.getComponent<TransformComponent>();
    
    // Spawn enemy plane mỗi 2 giây
    Uint32 currentTime = SDL_GetTicks();
    if (currentTime - lastEnemySpawnTime >= ENEMY_SPAWN_INTERVAL) {
        spawnEnemyPlane();
        lastEnemySpawnTime = currentTime;
    }

    // Tính toán vị trí spawn đạn từ giữa nhân vật
    int actualWidth = transform.width * transform.scale;
    int actualHeight = transform.height * transform.scale;
    float centerX = playerPos.x + (actualWidth / 2.0f);
    float centerY = playerPos.y + (actualHeight / 2.0f);
    
    const int PROJECTILE_SIZE = 40;
    float spawnX = centerX - (PROJECTILE_SIZE / 2.0f);
    float spawnY = centerY - (PROJECTILE_SIZE / 2.0f);

    // Kiểm tra thời gian để spawn projectile
    if (currentTime - lastProjectileTime >= PROJECTILE_INTERVAL) {
        std::cout << "Player position: " << playerPos.x << ", " << playerPos.y << std::endl;
        std::cout << "Spawning player_bullet at: " << spawnX << ", " << spawnY 
                 << " (Time: " << currentTime << ")" << std::endl;
        
        // Tạo player_bullet với 30 HP
        auto& player_bullet(manager.addEntity());
        player_bullet.addComponent<TransformComponent>(spawnX, spawnY, PROJECTILE_SIZE, PROJECTILE_SIZE, 1);
        player_bullet.addComponent<SpriteComponent>("res/graphic/main_bullet.png");
        player_bullet.addComponent<ProjectileComponent>(500, 1.0f, 0.0f);
        player_bullet.addComponent<ColliderComponent>("player_bullet");
        player_bullet.addComponent<HealthComponent>(30);
        player_bullet.addGroup(groupPlayers);
        
        lastProjectileTime = currentTime;
    }

    // Cập nhật tất cả entities
    manager.refresh();
    manager.update();

    // Kiểm tra va chạm giữa players và enemies
    auto& players = manager.getGroup(groupPlayers);
    auto& enemies = manager.getGroup(groupEnemies);

    for (auto& p : players) {
        if (!p->hasComponent<HealthComponent>()) continue;
        auto& playerHealth = p->getComponent<HealthComponent>();
        auto& playerCollider = p->getComponent<ColliderComponent>();

        for (auto& e : enemies) {
            if (!e->hasComponent<HealthComponent>()) continue;
            auto& enemyHealth = e->getComponent<HealthComponent>();
            auto& enemyCollider = e->getComponent<ColliderComponent>();

            if (Collision::AABB(playerCollider.collider, enemyCollider.collider)) {
                // Khi va chạm, cả hai bên đều nhận sát thương bằng HP của đối phương
                int playerHP = playerHealth.getCurrentHealth();
                int enemyHP = enemyHealth.getCurrentHealth();

                playerHealth.takeDamage(enemyHP);
                enemyHealth.takeDamage(playerHP);

                // Nếu enemy bị tiêu diệt, tăng điểm
                if (!enemyHealth.isAlive()) {
                    increaseScore(100);
                }
            }
        }
    }

    // Đảm bảo frame rate ổn định
    SDL_Delay(16); // Lock to ~60 FPS
}

void Game::eventsGame()
{
    SDL_PollEvent(&event);
    switch(event.type){
        case SDL_QUIT:
            gameRunning = false;
            break;

        default:
            break;
    }
}

void Game::CleanUp()
{
    if (scoreFont) {
        TTF_CloseFont(scoreFont);
    }
    TTF_Quit();
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
}

