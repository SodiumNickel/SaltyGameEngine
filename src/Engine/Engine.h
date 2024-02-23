#ifndef ENGINE_H
#define ENGINE_H

#include <SDL.h>
#include <imgui.h>
#include "Stage/Stage.h"

class Engine {
    private:
        bool isRunning; 
        SDL_Window* window;
        int width, height;
        SDL_Renderer* renderer;
        SDL_Texture* viewport;
        ImGuiWindowFlags dockspace_flags;

        Stage stage;

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