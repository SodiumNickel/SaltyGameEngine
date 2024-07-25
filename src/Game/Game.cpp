#include "Game.h"

#include <cstdint>
#include <iostream>
#include <memory>

#include <SDL.h>
#include <SDL_image.h>

#include "Game/ECS/ECS.h"
#include "Game/Helpers/JsonHelper.h" // TODO: might remove this
#include "Game/Components/TransformComponent.h"
#include "Game/Components/SpriteComponent.h"
#include "Game/Components/RigidbodyComponent.h"
// #include "../Components/BoxColliderComponent.h" might need for visual
#include "Game/Systems/RenderSystem.h"


Game::Game()
{
    // TODO: should initialize to scene that was last open, stored fps, etc.

    // Set to true on success of Initialize()
    isRunning = false;
}

Game::~Game()
{
    // Happens after clean up
}

// Returns 0 if successful
int Game::Initialize()
{
    // Init main SDL window
    if(SDL_Init(SDL_INIT_VIDEO) < 0) { return -1; }

    SDL_DisplayMode dm;
    SDL_GetDesktopDisplayMode(0, &dm);
    width = dm.w;
    height = dm.h - 50; // adjusted for border size, 75
    std::cout << width << ' ' << height << '\n';

    window = SDL_CreateWindow(
        "Game build", // TODO: should allow some more control over this, definitely name
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width, height,
        SDL_WINDOW_RESIZABLE
    );
    if (!window){
        SDL_Quit();
        return -1;
    }

    // Set app icon
    // TODO: should probably have a default
    // SDL_Surface* icon_surf = IMG_Load("EngineAssets/logotemp.png");
    // if(!icon_surf){std::cout << "failed icon_surf imgload";}
    // SDL_SetWindowIcon(window, icon_surf);

    renderer = SDL_CreateRenderer(window, -1, 0);
    if(!renderer){
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    registry = std::make_unique<Registry>();
    assetManager = std::make_unique<AssetManager>();

    isRunning = true;
    return 0;
}

// Game main loop
void Game::Run() 
{
    while(isRunning)
    {
        ProcessInput();
        Update();
        Render();

        // NOTE: was having issues with high gpu usage before, will still allow for a (pseudo) uncapped frame rate in settings
        // Limits frame rate to 1/targetFrameTime 
        // uint64_t curFrameTime = SDL_GetTicks64();
        // uint64_t deltaTime = curFrameTime - GameData->prevFrameTime;
        // if(deltaTime < engineData->targetFrameTime) SDL_Delay(engineData->targetFrameTime - deltaTime);
        // engineData->prevFrameTime = SDL_GetTicks64();
    }
}

void Game::ProcessInput()
{
    SDL_Event e;
    while(SDL_PollEvent(&e)){
        switch(e.type){
            case SDL_QUIT:
                isRunning = false;
                break; // TODO: all of these do them every frame, should only do it on first frame down, wait until up
            case SDL_KEYDOWN: // TODO: implement proper key detection, and use it here instead
                break;
        }
    }
}

void Game::Update()
{
    // TODO: Check for events here
    // TODO: probably call script updates here?
    registry->Update();
}

void Game::Render()
{
    // Background color
    SDL_SetRenderDrawColor(renderer, 40, 40, 100, 255);
    SDL_RenderClear(renderer);

    // Allows resizing of viewport, both by boundaries and zoom
    registry->GetSystem<RenderSystem>().Update(renderer, assetManager, glm::vec2(0,0));

    SDL_RenderPresent(renderer);
}

// Clean up
void Game::Destroy()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}