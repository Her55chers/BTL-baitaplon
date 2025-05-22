#include "core/Game.h"
#include "core/Texture.h"
#include "core/Map.h"
#include "core/Vector2D.h"
#include "core/Collision.h"

#include "gameObject/Components.h"


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
    groupMap,
    groupMisc
};

// Thêm biến đếm số enemies bị tiêu diệt
int destroyedEnemiesCount = 0;

Game::Game()
{
    gameRunning = false;
    window = nullptr;
    isGameOver = false;
    gameOverScreen = nullptr;
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
        scoreFont = TTF_OpenFont("res/fonts/Roboto-Regular.ttf", 24);
        if(!scoreFont) {
            std::cout << "Font loading failed: " << TTF_GetError() << std::endl;
            gameRunning = false;
            return;
        }

        gameRunning = true;
    }
    else{
        std::cout << "Khoi tao khong thanh cong!" << std::endl;
        gameRunning = false;
    }

    map = new Map();

    // Initialize player with 100 HP
    player.addComponent<TransformComponent>(0.0f, 0.0f, 40, 40, 2);
    player.addComponent<SpriteComponent>("res/graphic/main_char.png");
    player.addComponent<Keyboard>();
    player.addComponent<ColliderComponent>("player");
    player.addComponent<HealthComponent>(10);
    player.addGroup(groupPlayers);

    // Thêm cây vào map
    auto& tree(manager.addEntity());
    tree.addComponent<TransformComponent>(280.0f, 40.0f, 40, 40, 13); // x=280, y=40, scale=13
    tree.addComponent<SpriteComponent>("res/graphic/tree.png");
    tree.addGroup(groupMap);

    resetScore();
}

void Game::spawnEnemyPlane()
{
    float spawnX = 1280.0f;
    const int SCREEN_HEIGHT = 720;
    const int MAX_SPAWN_HEIGHT = (SCREEN_HEIGHT * 3) / 5;
    float spawnY = rand() % MAX_SPAWN_HEIGHT;

    auto& enemy_plane(manager.addEntity());
    enemy_plane.addComponent<TransformComponent>(spawnX, spawnY, 40, 40, 1);
    enemy_plane.addComponent<SpriteComponent>("res/graphic/enemy_plane.png");
    enemy_plane.addComponent<ColliderComponent>("enemy_plane");
    // HP scales with level
    enemy_plane.addComponent<HealthComponent>(30 * currentLevel);
    enemy_plane.addComponent<EnemyMovementComponent>(300.0f);
    enemy_plane.addGroup(groupEnemies);

    std::cout << "Enemy plane spawned at (" << spawnX << ", " << spawnY
              << ") with " << (30 * currentLevel) << " HP (Level " << currentLevel << ")" << std::endl;
}

void Game::spawnEnemyTank()
{
    float spawnX = 1280.0f;
    float spawnY = 520.0f;

    auto& enemy_tank(manager.addEntity());
    enemy_tank.addComponent<TransformComponent>(spawnX, spawnY, 40, 40, 1);
    enemy_tank.addComponent<SpriteComponent>("res/graphic/enemy_tank.png");
    enemy_tank.addComponent<ColliderComponent>("enemy_tank");
    // HP scales with level
    enemy_tank.addComponent<HealthComponent>(60 * currentLevel);
    enemy_tank.addComponent<EnemyMovementComponent>(120.0f);
    enemy_tank.addGroup(groupEnemies);

    std::cout << "Enemy tank spawned at (" << spawnX << ", " << spawnY
              << ") with " << (60 * currentLevel) << " HP (Level " << currentLevel << ")" << std::endl;
}

void Game::spawnEnemyBullet(float startX, float startY)
{
    // Lấy vị trí hiện tại của player
    Vector2D playerPos = player.getComponent<TransformComponent>().position;

    // Tính vector hướng từ đạn đến player
    Vector2D direction = Vector2D(
        playerPos.x - startX,
        playerPos.y - startY
    );

    // Chuẩn hóa vector (để tổng bình phương = 1)
    float length = sqrt(direction.x * direction.x + direction.y * direction.y);
    if (length > 0) {
        direction.x = direction.x / length;
        direction.y = direction.y / length;
    }

    auto& enemy_bullet(manager.addEntity());
    enemy_bullet.addComponent<TransformComponent>(startX, startY, 20, 20, 2); // Tăng scale lên 2
    enemy_bullet.addComponent<SpriteComponent>("res/graphic/enemy_tracking_bullet.png");
    enemy_bullet.addComponent<ProjectileComponent>(350.0f, direction.x, direction.y); // Giảm tốc độ xuống 350.0f
    enemy_bullet.addComponent<ColliderComponent>("enemy_bullet");
    enemy_bullet.addComponent<HealthComponent>(25);
    enemy_bullet.addGroup(groupEnemies);

    std::cout << "Enemy bullet spawned at (" << startX << ", " << startY << ") targeting player at ("
              << playerPos.x << ", " << playerPos.y << ")" << std::endl;
}

auto& players(manager.getGroup(groupPlayers));

void Game::increaseScore(int amount)
{
    currentScore += amount;
    std::cout << "Score increased by " << amount << ". Current score: " << currentScore << std::endl;
}

void Game::renderScore()
{
    // Luôn hiển thị điểm số, kể cả khi = 0
    std::string scoreText = "Score: " + std::to_string(currentScore);
    std::string levelText = "Level: " + std::to_string(currentLevel);

    // Lấy máu của người chơi
    int playerHP = 0;
    if (player.hasComponent<HealthComponent>()) {
        playerHP = player.getComponent<HealthComponent>().getCurrentHealth();
    }
    std::string hpText = "HP: " + std::to_string(playerHP);

    // Đảm bảo font đã được load
    if (!scoreFont) {
        std::cout << "Warning: Font not loaded for score rendering" << std::endl;
        return;
    }

    // Màu cho các text
    SDL_Color whiteColor = {255, 255, 255, 255}; // Màu trắng cho Score và Level
    SDL_Color greenColor = {0, 255, 0, 255};     // Màu xanh lá cho HP

    // Render Score (màu trắng)
    SDL_Surface* scoreSurface = TTF_RenderText_Solid(scoreFont, scoreText.c_str(), whiteColor);
    if (scoreSurface == nullptr) {
        std::cout << "Unable to render score surface: " << TTF_GetError() << std::endl;
        return;
    }

    SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
    SDL_Rect scoreRect = { 10, 10, scoreSurface->w, scoreSurface->h };
    SDL_FreeSurface(scoreSurface);

    // Render Level (màu trắng)
    SDL_Surface* levelSurface = TTF_RenderText_Solid(scoreFont, levelText.c_str(), whiteColor);
    if (levelSurface == nullptr) {
        SDL_DestroyTexture(scoreTexture);
        std::cout << "Unable to render level surface: " << TTF_GetError() << std::endl;
        return;
    }

    SDL_Texture* levelTexture = SDL_CreateTextureFromSurface(renderer, levelSurface);
    SDL_Rect levelRect = { 200, 10, levelSurface->w, levelSurface->h }; // Đặt bên phải Score
    SDL_FreeSurface(levelSurface);

    // Render HP (màu xanh lá)
    SDL_Surface* hpSurface = TTF_RenderText_Solid(scoreFont, hpText.c_str(), greenColor);
    if (hpSurface == nullptr) {
        SDL_DestroyTexture(scoreTexture);
        SDL_DestroyTexture(levelTexture);
        std::cout << "Unable to render HP surface: " << TTF_GetError() << std::endl;
        return;
    }

    SDL_Texture* hpTexture = SDL_CreateTextureFromSurface(renderer, hpSurface);
    SDL_Rect hpRect = { 10, 40, hpSurface->w, hpSurface->h }; // Hiển thị dưới Score
    SDL_FreeSurface(hpSurface);

    // Render tất cả textures
    SDL_RenderCopy(renderer, scoreTexture, NULL, &scoreRect);
    SDL_RenderCopy(renderer, levelTexture, NULL, &levelRect);
    SDL_RenderCopy(renderer, hpTexture, NULL, &hpRect);

    // Cleanup
    SDL_DestroyTexture(scoreTexture);
    SDL_DestroyTexture(levelTexture);
    SDL_DestroyTexture(hpTexture);
}

void Game::renderGame()
{
    SDL_RenderClear(renderer);

    if (isGameOver && gameOverScreen != nullptr) {
        // Debug output
        std::cout << "Rendering game over screen..." << std::endl;
        gameOverScreen->draw();
    } else {
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

        for(auto& m : manager.getGroup(groupMisc))
        {
            m->draw();
        }

        // Render score
        renderScore();
    }

    SDL_RenderPresent(renderer);
}

void Game::spawnEnemyDrone(float centerX, float centerY, float angleOffset)
{
    auto& enemy_drone(manager.addEntity());
    enemy_drone.addComponent<TransformComponent>(centerX, centerY, 30, 30, 1); // Kích thước nhỏ hơn boss
    enemy_drone.addComponent<SpriteComponent>("res/graphic/enemy_drone.png");
    enemy_drone.addComponent<ColliderComponent>("enemy_drone");
    enemy_drone.addComponent<HealthComponent>(20 * currentLevel); // 20 HP * level

    // Thêm component chuyển động quay với góc bắt đầu khác nhau cho mỗi drone
    DroneMovementComponent& droneMovement = enemy_drone.addComponent<DroneMovementComponent>(80.0f, angleOffset, 2.0f);
    droneMovement.setCenterPoint(centerX, centerY);

    enemy_drone.addGroup(groupEnemies);

    std::cout << "Enemy drone spawned around (" << centerX << ", " << centerY
              << ") with " << (20 * currentLevel) << " HP (Level " << currentLevel << ")" << std::endl;
}

void Game::spawnEnemyBoss()
{
    float spawnX = 1100.0f;
    float spawnY = 80.0f + (rand() % (400 - 80));

    auto& enemy_boss(manager.addEntity());
    enemy_boss.addComponent<TransformComponent>(spawnX, spawnY, 60, 60, 1.5);
    enemy_boss.addComponent<SpriteComponent>("res/graphic/enemy_boss.png");
    enemy_boss.addComponent<ColliderComponent>("enemy_boss");
    enemy_boss.addComponent<HealthComponent>(180 * currentLevel);
    enemy_boss.addComponent<EnemyMovementComponent>(200.0f, true);
    enemy_boss.addGroup(groupEnemies);

    std::cout << "Enemy boss spawned at (" << spawnX << ", " << spawnY
              << ") with " << (180 * currentLevel) << " HP (Level " << currentLevel << ")" << std::endl;

    // Spawn drones around boss
    int numDrones = currentLevel; // Số lượng drone bằng level hiện tại
    float angleStep = 2 * M_PI / numDrones; // Chia đều góc cho các drone

    for (int i = 0; i < numDrones; i++) {
        float angleOffset = i * angleStep; // Góc bắt đầu cho mỗi drone
        spawnEnemyDrone(spawnX, spawnY, angleOffset);
    }
}

void Game::updateGame()
{
    if (isGameOver) {
        return; // Không cập nhật gì nếu đã game over
    }

    // Cập nhật trạng thái game
    Vector2D playerPos = player.getComponent<TransformComponent>().position;
    auto& transform = player.getComponent<TransformComponent>();
    auto& playerHealth = player.getComponent<HealthComponent>();

    // Kiểm tra game over
    if (playerHealth.getCurrentHealth() <= 0 && !isGameOver) {
        std::cout << "Game Over triggered! Player HP: " << playerHealth.getCurrentHealth() << std::endl;
        
        // Xóa tất cả entities hiện tại
        manager.clearAllEntities();

        // Tạo màn hình Game Over fullscreen (1280x720)
        auto& newGameOver = manager.addEntity();
        gameOverScreen = &newGameOver;
        gameOverScreen->addComponent<TransformComponent>(0.0f, 0.0f, 1280, 720, 1);
        
        // Thêm sprite component với debug output
        auto& spriteComp = gameOverScreen->addComponent<SpriteComponent>("res/graphic/Game_over.png", true);
        std::cout << "Game Over screen created at position (0,0) with size 1280x720" << std::endl;
        
        gameOverScreen->addGroup(groupMisc);
        isGameOver = true;
        return;
    }

    Uint32 currentTime = SDL_GetTicks();

    // Check if boss exists and get boss position
    bool bossExists = false;
    Vector2D bossPosition;
    auto& enemies = manager.getGroup(groupEnemies);
    for (auto& e : enemies) {
        if (e->hasComponent<ColliderComponent>() &&
            e->getComponent<ColliderComponent>().tag == "enemy_boss") {
            bossExists = true;
            bossPosition = e->getComponent<TransformComponent>().position;
            break;
        }
    }

    // Update drone positions to orbit around current boss position
    if (bossExists) {
        for (auto& e : enemies) {
            if (e->hasComponent<ColliderComponent>() &&
                e->getComponent<ColliderComponent>().tag == "enemy_drone") {
                auto& droneMovement = e->getComponent<DroneMovementComponent>();
                droneMovement.setCenterPoint(bossPosition.x, bossPosition.y);
            }
        }
    }

    // Boss shooting and movement logic
    if (bossExists) {
        static Uint32 lastBossShootTime = SDL_GetTicks();
        static Uint32 lastBossSpeedChangeTime = SDL_GetTicks();
        if (currentTime - lastBossShootTime >= 500) { // Every half second (500ms)
            for (auto& e : enemies) {
                if (e->hasComponent<ColliderComponent>() &&
                    e->getComponent<ColliderComponent>().tag == "enemy_boss") {

                    // 40% chance to shoot
                    if (rand() % 100 < 40) {
                        auto& bossTransform = e->getComponent<TransformComponent>();
                        float bulletX = bossTransform.position.x;
                        float bulletY = bossTransform.position.y + bossTransform.height/2;
                        spawnEnemyBullet(bulletX, bulletY);
                    }

                    // Update boss movement
                    auto& bossTransform = e->getComponent<TransformComponent>();
                    auto& bossMovement = e->getComponent<EnemyMovementComponent>();
                    if (bossTransform.position.y <= 40 || bossTransform.position.y >= 480) {
                        // Reverse vertical direction when hitting boundaries
                        bossMovement.velocity.y *= -1;
                    }

                    // Change boss speed every second
                    if (currentTime - lastBossSpeedChangeTime >= 1000) {
                        float newSpeed = 180.0f + (rand() % 221); // 180 + random(0-220) = 180-400
                        bossMovement.setSpeed(newSpeed);
                        std::cout << "Boss speed changed to: " << newSpeed << std::endl;
                        lastBossSpeedChangeTime = currentTime;
                    }
                }
            }
            lastBossShootTime = currentTime;
        }
    }

    // Tank shooting logic - only if no boss exists
    if (!bossExists) {
        static Uint32 lastTankShootTime = SDL_GetTicks();
        if (currentTime - lastTankShootTime >= 2000) { // Every 2 seconds
            for (auto& e : enemies) {
                if (e->hasComponent<ColliderComponent>() &&
                    e->getComponent<ColliderComponent>().tag == "enemy_tank") {

                    // 60% chance to shoot
                    if (rand() % 100 < 60) {
                        auto& tankTransform = e->getComponent<TransformComponent>();
                        float bulletX = tankTransform.position.x;
                        float bulletY = tankTransform.position.y + tankTransform.height/2;
                        spawnEnemyBullet(bulletX, bulletY);
                    }
                }
            }
            lastTankShootTime = currentTime;
        }
    }

    // Spawn regular enemies only if boss doesn't exist
    if (currentTime - lastEnemySpawnTime >= ENEMY_SPAWN_INTERVAL) {
        // Kiểm tra điểm số để quyết định spawn boss
        bool shouldSpawnBoss = false;

        // Tính mốc điểm số tiếp theo cần đạt (5000 * level)
        static int nextBossScoreMilestone = 500;

        // Kiểm tra xem điểm số hiện tại đã vượt qua mốc chưa
        if (currentScore >= nextBossScoreMilestone) {
            shouldSpawnBoss = true;
            // Cập nhật mốc điểm tiếp theo dựa trên level hiện tại
            nextBossScoreMilestone = 5000 * (currentLevel + 1);
            std::cout << "Next boss will spawn at score: " << nextBossScoreMilestone << std::endl;
        }

        if (shouldSpawnBoss && !bossExists) {
            spawnEnemyBoss();
            std::cout << "Boss spawned at score: " << currentScore << "! (Level " << currentLevel << ")" << std::endl;
        } else if (!bossExists) { // Only spawn regular enemies if no boss exists
            // Spawn regular enemies
            if (rand() % 2 == 0) {
                spawnEnemyPlane();
            }
            if (rand() % 4 == 0) {
                spawnEnemyTank();
            }
        }
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

    // Kiểm tra va chạm giữa player và apple
    auto& misc = manager.getGroup(groupMisc);
    for (auto& m : misc) {
        if (m->hasComponent<ColliderComponent>() &&
            m->getComponent<ColliderComponent>().tag == "apple") {

            if (Collision::AABB(player.getComponent<ColliderComponent>().collider,
                              m->getComponent<ColliderComponent>().collider)) {
                // Tăng HP cho player
                auto& playerHealth = player.getComponent<HealthComponent>();
                playerHealth.heal(30);
                // Tăng điểm số
                increaseScore(100);
                // Xóa apple
                m->destroy();
                std::cout << "Player collected apple! HP increased by 30, Score increased by 100" << std::endl;
            }
        }
    }

    // Cập nhật tất cả entities
    manager.refresh();
    manager.update();

    // Kiểm tra va chạm giữa players và enemies
    auto& players = manager.getGroup(groupPlayers);

    for (auto& p : players) {
        if (!p->hasComponent<HealthComponent>()) continue;
        auto& playerHealth = p->getComponent<HealthComponent>();
        auto& playerCollider = p->getComponent<ColliderComponent>();

        for (auto& e : enemies) {
            if (!e->hasComponent<HealthComponent>()) continue;
            auto& enemyHealth = e->getComponent<HealthComponent>();
            auto& enemyCollider = e->getComponent<ColliderComponent>();

            if (Collision::AABB(playerCollider.collider, enemyCollider.collider)) {
                int playerHP = playerHealth.getCurrentHealth();
                int enemyHP = enemyHealth.getCurrentHealth();

                playerHealth.takeDamage(enemyHP);
                enemyHealth.takeDamage(playerHP);

                if (!enemyHealth.isAlive()) {
                    destroyedEnemiesCount++;

                    // Kiểm tra nếu boss bị tiêu diệt
                    if (enemyCollider.tag == "enemy_boss") {
                        increaseLevel();  // Tăng level khi đánh bại boss
                    }

                    // Kiểm tra số enemies bị tiêu diệt
                    if (destroyedEnemiesCount >= 10) {
                        spawnApple();
                        spawnApple();
                        spawnApple();
                        destroyedEnemiesCount = 0;
                    }

                    // Tăng điểm theo loại enemy và level
                    if (enemyCollider.tag == "enemy_tank") {
                        increaseScore(150 * currentLevel);
                    } else if (enemyCollider.tag == "enemy_boss") {
                        increaseScore(500 * currentLevel);
                    } else if (enemyCollider.tag == "enemy_drone") {
                        increaseScore(80 * currentLevel);
                    } else {
                        increaseScore(100 * currentLevel);
                    }
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

void Game::spawnApple()
{
    // Random vị trí spawn trong khoảng cho phép
    float spawnX = 280.0f + (rand() % (int)(800.0f - 280.0f));
    float spawnY = 40.0f + (rand() % (int)(340.0f - 40.0f));

    auto& apple(manager.addEntity());
    apple.addComponent<TransformComponent>(spawnX, spawnY, 40, 40, 1);
    apple.addComponent<SpriteComponent>("res/graphic/apple.png");
    apple.addComponent<ColliderComponent>("apple");
    apple.addGroup(groupMisc);

    std::cout << "Apple spawned at (" << spawnX << ", " << spawnY << ")" << std::endl;
}

int Game::getCurrentScore() const
{
    return currentScore;
}

void Game::resetScore()
{
    currentScore = 0;
}

int Game::getCurrentLevel() const
{
    return currentLevel;
}

void Game::increaseLevel()
{
    currentLevel++;
    std::cout << "Level increased to: " << currentLevel << "!" << std::endl;
}

void Game::resetLevel()
{
    currentLevel = 1;
}

