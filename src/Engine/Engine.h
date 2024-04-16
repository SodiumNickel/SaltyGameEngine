#ifndef ENGINE_H
#define ENGINE_H

#include <SDL.h>
#include <imgui.h>
#include "EngineData.h"
#include "Stage/Stage.h"
#include "History/EditHistory.h"
#include "EnGUI.h"
#include <vector>

class Engine {
    private:
        bool isRunning; 
        SDL_Window* window;
        int width, height;
        SDL_Renderer* renderer;
        SDL_Texture* viewport;
        ImGuiWindowFlags dockspace_flags;

        // Shared pointer used to pass engine data to tabs, stage, edit history
        std::shared_ptr<EngineData> engineData;
        
        std::shared_ptr<EditHistory> editHistory;
        std::shared_ptr<Stage> stage;
        std::unique_ptr<Menu> menu;
        std::vector<Tab*> openTabs;
    public:
        Engine();
        ~Engine();
        int Initialize();
        void Run();
        void ProcessInput();
        void UpdateViewport();
        void UpdateGUI();
        void Render();
        void Destroy();
};

#endif