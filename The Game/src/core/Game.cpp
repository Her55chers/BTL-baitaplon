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

//std::vector<ColliderComponent*> Game::colliders;

auto& player(manager.addEntity());
auto& tree(manager.addEntity());

/*&auto& tile0(manager.addEntity());
auto& tile1(manager.addEntity());
auto& tile2(manager.addEntity());*/

Game::Game()
{
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
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

        gameRunning = true;
    }
    else{
        std::cout << "Khoi tao khong thanh cong!" << std::endl;
        gameRunning = false;
    }

    map = new Map();
    //
    /*tile0.addComponent<TileComponent>(0, 560, 40, 40, 1);
    tile1.addComponent<TileComponent>(40, 560, 40, 40, 1);
    tile2.addComponent<TileComponent>(80, 560, 40, 40, 1);

    tile0.addComponent<ColliderComponent>("grass");
    tile1.addComponent<ColliderComponent>("grass");
    tile2.addComponent<ColliderComponent>("grass");*/

    player.addComponent<TransformComponent>(0.0f, 0.0f, 40, 40, 2);
    player.addComponent<SpriteComponent>("res/graphic/main_char.png");
    player.addComponent<Keyboard>();
    player.addComponent<ColliderComponent>("player");

    tree.addComponent<TransformComponent>(200.0f, 200.0f, 40, 40, 2);
    tree.addComponent<SpriteComponent>("res/graphic/Enemy_boss.png");
    tree.addComponent<ColliderComponent>("tree");
}

void Game::renderGame()
{
    SDL_RenderClear(renderer);

    map->DrawMap();
    manager.draw();

    SDL_RenderPresent(renderer);
}
void Game::updateGame()
{
    Vector2D playerPos = player.getComponent<TransformComponent>().position;

	manager.refresh();
	manager.update();

	if (Collision::AABB(player.getComponent<ColliderComponent>().collider, tree.getComponent<ColliderComponent>().collider)) {
		player.getComponent<TransformComponent>().position = playerPos;
		std::cout << "Collision!" << std::endl;
	}
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
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
}

