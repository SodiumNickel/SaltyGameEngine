#ifndef ENGINE_H
#define ENGINE_H

#include <memory>
#include <vector>

#include <SDL.h>
#include <imgui.h>
#include <soloud.h>
#include <soloud_wav.h> // TEMP

#include "Engine/EngineData.h"
#include "Engine/History/EditHistory.h"
#include "Engine/Menu/Menu.h"
#include "Engine/Stage/Stage.h"
#include "Engine/Tabs/Tabs.h"

#include "Game/ECS/ECS.h"
#include "Game/AssetManager/AssetManager.h"
#include "Game/Salty/SaltyAudio.h" // TODO: TEMP

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

        // Will be stored in Game along with other vars in Stage
        std::shared_ptr<Registry> registry; 
        std::shared_ptr<AssetManager> assetManager;

        std::shared_ptr<EditHistory> editHistory;
        // For shortcutted input handling (i.e. Undo)
        bool lCtrlHeld;
        bool rCtrlHeld;

        Sound sound;

        std::shared_ptr<Stage> stage;
        std::unique_ptr<Menu> menu;
        std::vector<std::unique_ptr<ITab>> openTabs;
    public:
        Engine();
        ~Engine();
        int Initialize();
        void Run();
        void ProcessInput(); // TODO: some of this can def be private, not sure thats a big issue though
        void KeyDownInput(SDL_Scancode scancode);
        void UpdateViewport();
        void UpdateGUI();
        void Render();
        void Destroy();
};

#endif // ENGINE_H