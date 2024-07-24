#ifndef GAME_H
#define GAME_H

#include <memory>
#include <vector>

#include <SDL.h>
#include <imgui.h>
#include <json.hpp>
using json = nlohmann::json;

#include "Game/ECS/ECS.h"
#include "Game/AssetManager/AssetManager.h"


class Game {
    private:
        bool isRunning; 
        SDL_Window* window;
        int width, height;
        SDL_Renderer* renderer;

        std::unique_ptr<Registry> registry; 
        std::unique_ptr<AssetManager> assetManager;

        void CreateEntityTree(json jEntities, json jRootIds);
    public:
        Game();
        ~Game();
        int Initialize();
        void Run();
        void ProcessInput();
        void Update();
        void Render();
        void Destroy();
};


#endif // GAME_H