#include "Engine/Engine.h"

#include <cstdint>
#include <iostream>

#include <SDL.h>
#include <SDL_image.h>
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>

// TODO: this isnt used rn
#include "Engine/Debug/SaltyDebug.h"
#include "Engine/EngineData.h"
#include "Engine/History/EditHistory.h"
#include "Engine/Menu/Menu.h"
#include "Engine/Tabs/Tabs.h"


// Constructor
Engine::Engine()
{
    // TODO: should initialize to scene that was last open, stored fps, etc.
    engineData = std::make_shared<EngineData>(0, 8);
    editHistory = std::make_shared<EditHistory>(engineData); // TODO: could move this into initialize, especially if it needs scene
    // Set to true on success of Initialize()
    isRunning = false;
}

// Destructor
Engine::~Engine()
{
    // Happens after clean up
}

// Returns 0 if successful
int Engine::Initialize()
{
    // Init main SDL window
    if(SDL_Init(SDL_INIT_VIDEO) < 0) { return -1; }

    SDL_DisplayMode dm;
    SDL_GetDesktopDisplayMode(0, &dm);
    width = dm.w;
    height = dm.h - 50; // adjusted for border size, 75
    std::cout << width << ' ' << height << '\n';

    window = SDL_CreateWindow(
        "Salty Game Engine",
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
    SDL_Surface* icon_surf = IMG_Load("EngineAssets/logotemp.png"); // TODO: make engine specific logo
    if(!icon_surf){std::cout << "failed icon_surf imgload";}
    SDL_SetWindowIcon(window, icon_surf);

    // Create renderer for Stage
    renderer = SDL_CreateRenderer(window, -1, 0);
    if(!renderer){
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // Create render target for viewport
    viewport = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        800, 800
    );

    // Create registry
    registry = std::make_shared<Registry>();
    assetManager = std::make_shared<AssetManager>();

    // Create stage with access to renderer/viewport
    stage = std::make_shared<Stage>(engineData, registry, assetManager);
    stage->Initialize(renderer, viewport);

    // Init imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); 
    
    // Set ImGuiIO Flags
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigDragClickToInputText = true;
    (void)io;

    // Set ImGui DockSpace (window) Flags
    dockspace_flags = ImGuiWindowFlags_NoDocking;
    dockspace_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    dockspace_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    // Set up with SDL2
    ImGui_ImplSDL2_InitForSDLRenderer(
        window,
        renderer
    );
    ImGui_ImplSDLRenderer2_Init(renderer);

    menu = std::make_unique<Menu>(registry, engineData, editHistory);

    // Open initial tabs
    openTabs.push_back(new EntityTab(engineData, editHistory, registry)); // TODO: this should be better pointers(?) tabs i mean
    openTabs.push_back(new ComponentTab(engineData, editHistory, registry, assetManager)); // TODO: unify order of this
    openTabs.push_back(new ScriptTab(registry));
    openTabs.push_back(new AssetTab(registry));
    openTabs.push_back(new LogTab(registry));

    isRunning = true;
    return 0;
}

// Engine main loop
void Engine::Run() 
{
    while(isRunning)
    {
        ProcessInput();
        UpdateGUI();
        Render();

        // NOTE: was having issues with high gpu usage before, will still allow for a (pseudo) uncapped frame rate in settings
        // Limits frame rate to 1/targetFrameTime 
        uint64_t curFrameTime = SDL_GetTicks64();
        uint64_t deltaTime = curFrameTime - engineData->prevFrameTime;
        if(deltaTime < engineData->targetFrameTime) SDL_Delay(engineData->targetFrameTime - deltaTime);
        engineData->prevFrameTime = SDL_GetTicks64();
    }
}

void Engine::ProcessInput()
{
    SDL_Event e;
    while(SDL_PollEvent(&e)){
        switch(e.type){
            case SDL_QUIT:
                isRunning = false;
                break; // TODO: all of these do them every frame, should only do it on first frame down, wait until up
            case SDL_KEYDOWN: // TODO: implement proper key detection, and use it here instead
                if(editHistory->canUndo && e.key.keysym.sym == SDLK_z) editHistory->Undo();
                else if(editHistory->canRedo && e.key.keysym.sym == SDLK_y) editHistory->Redo();
                else if(editHistory->unsaved && e.key.keysym.sym == SDLK_s) editHistory->Save();
                break;
        }
        ImGui_ImplSDL2_ProcessEvent(&e);
    }
}

void Engine::UpdateGUI()
{
    // Full window background (hidden)
    SDL_SetRenderDrawColor(renderer, 120, 180, 255, 255);
    SDL_RenderClear(renderer);

    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame(window);
    SDL_GetWindowSize(window, &width, &height);
    ImGui::NewFrame();

    // Dockspace
    // Scale dockspace to full window
    ImGui::SetNextWindowPos(ImVec2(0,18)); // adjusted for menu bar
    ImGui::SetNextWindowSize(ImVec2(width,height-18));
    // Remove padding from dockspace
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace", nullptr, dockspace_flags);
    
    ImGui::PopStyleVar(3);

    // Submit the DockSpace
    ImGuiID dockspace_id = ImGui::GetID("FullDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

    // Top menu bar
    menu->Begin();

    // Stage
    ImGui::Begin("Stage");
    
    ImVec2 stageSize = ImGui::GetWindowSize();
    stageSize.x -= 16; // makes all borders equal size
    stageSize.y -= 35; // adjusted for tab bar, hides scroll

    stage->stageSize = stageSize;
    ImGui::Image((ImTextureID)viewport, stageSize);
    UpdateViewport();

    ImGui::End();

    // TODO: dont allow hide tab bar, also stop highlighting it when it is clicked on
    // Draw all tabs that are open
    for (ITab* tab : openTabs){
        tab->Begin();
    }

    ImGui::ShowDemoWindow();

    // End dockspace
    ImGui::End();
}

void Engine::UpdateViewport()
{
    // TODO: should either call stage or game render system
    // create stage earlier, pass renderer, viewport, then they can do whatever

    // if in stage
    if(true){
        stage->Run();
    }
}

void Engine::Render()
{
    ImGui::Render();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());

    SDL_RenderPresent(renderer);
}

// Clean up
void Engine::Destroy()
{
    auto it = openTabs.begin();
    while(it != openTabs.end()){
        delete *it;
        // no need to avoid dangling ptr, as we are erasing from vector 
        it = openTabs.erase(it);
    }

    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyTexture(viewport);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}