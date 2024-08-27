#include "Engine/Engine.h"

#include <cstdint>
#include <iostream>
#include <fstream>
#include <memory>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include <soloud.h>
#include <json.hpp>
using json = nlohmann::json;

// TODO: this isnt used rn
#include "Game/Salty/SaltyDebug.h"
#include "Engine/EngineData.h"
#include "Engine/Altered/EngineAssetManager.h"
#include "Engine/History/EditHistory.h"
#include "Engine/Menu/Menu.h"
#include "Engine/Tabs/Tabs.h"

#include "Game/Salty/SaltyAudio.h"

Engine::Engine()
{
    // TODO: should initialize to scene that was last open, stored fps, etc.
    engineData = std::make_shared<EngineData>();
    editHistory = std::make_shared<EditHistory>(engineData); // TODO: could move this into initialize, especially if it needs scene
    
    // Set to true on success of Initialize()
    isRunning = false;
}

Engine::~Engine()
{
    // Happens after clean up
}

// Returns 0 if successful
int Engine::Initialize()
{
    // Handling creation before window is opened (so we do not sit on a blank screen)
    registry = std::make_shared<Registry>();
    assetManager = std::make_shared<EngineAssetManager>(engineData);
    Audio::soloud.init();

    // NOTE: These will be rendered in Engine::UpdateGUI() so no need to worry about ImGui not being initialized
    // Will be initialized with renderer and viewport below, this also creates the initial scene in registry
    stage = std::make_shared<Stage>(engineData, registry, assetManager, editHistory);
    // Menu bar on top of screen
    menu = std::make_unique<Menu>(registry, engineData, editHistory);
    // Open initial tabs
    openTabs.push_back(std::make_unique<EntityTab>(engineData, editHistory, registry));
    openTabs.push_back(std::make_unique<ComponentTab>(engineData, editHistory, registry, assetManager)); // TODO: unify order of this
    openTabs.push_back(std::make_unique<ScriptTab>(engineData, editHistory, registry));
    openTabs.push_back(std::make_unique<AssetTab>(registry, engineData));
    openTabs.push_back(std::make_unique<LogTab>(registry));

    // Init fonts/ttf
    // if(TTF_Init() == -1) { return -1; } // TODO: do i need to init video before this?? idk if this does anything

    // Main window appears now
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
    // TODO: do i have to free surface here?

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

    // Give stage with access to renderer/viewport
    // TODO: The LoadScene here can be moved earlier if we dont load the assets, can optimize asset loading in general
    stage->Initialize(renderer, viewport);
    max = glm::max(width, height);
    stage->stageSize = max;

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

    // Add all scripts to EngineData
    std::ifstream f("Projects/" + engineData->projectName + + "/Unique/scripts.json");
    json jScripts = json::parse(f)["filepaths"];
    f.close();
    for(int scriptIdx = 0; scriptIdx < jScripts.size(); scriptIdx++){
        engineData->scriptFilepaths.push_back(jScripts[scriptIdx].get<std::string>());
    }

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
    SDL_Event event;
    while(SDL_PollEvent(&event)){
        switch(event.type){
            case SDL_QUIT:
                isRunning = false;
                break;
            case SDL_KEYDOWN:
                if(event.key.repeat == 0){
                    // Detect either control key being pressed
                    if(event.key.keysym.scancode == SDL_SCANCODE_LCTRL) lCtrlHeld = true;
                    else if(event.key.keysym.scancode == SDL_SCANCODE_RCTRL) rCtrlHeld = true;
                    // If either control key is held, precondition of KeyDownInput is true, and should detect for shortcut
                    else if(lCtrlHeld || rCtrlHeld) KeyDownInput(event.key.keysym.scancode);
                }
                break;
            case SDL_KEYUP:
                // Detect either control key being released
                if(event.key.keysym.scancode == SDL_SCANCODE_LCTRL) lCtrlHeld = false;
                else if(event.key.keysym.scancode == SDL_SCANCODE_RCTRL) rCtrlHeld = false;
                break;
            default:
                break;
        }
        ImGui_ImplSDL2_ProcessEvent(&event);
    }

    // Check if there were any changes to script SF_ vars (in header files)
    
}
// Pre: One of the ctrl keys is held down -> potential for a shortcut
void Engine::KeyDownInput(SDL_Scancode scancode){
    switch(scancode){
        case SDL_SCANCODE_Z:
            if(editHistory->canUndo) editHistory->Undo();
            break;
        case SDL_SCANCODE_Y:
            if(editHistory->canRedo) editHistory->Redo();
            break;
        case SDL_SCANCODE_S:
            if(editHistory->unsaved) editHistory->Save();
            break;
        default:
            break;
    }
}

void Engine::UpdateGUI()
{
    // Full window background (hidden)
    SDL_SetRenderDrawColor(renderer, 120, 180, 255, 255); // TODO: should probably allow user to select bg color, probably never used tbh
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
    //ImGui::SetNextWindowSizeConstraints(ImVec2(200, 200), ImVec2(FLT_MAX, FLT_MAX));
    
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0)); // Remove padding (for viewport)
    ImGui::Begin("Stage", NULL, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_MenuBar);
    
    ImGui::PopStyleVar(); // Restore padding for menu
    stage->Menu();
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0)); // Remove padding (for viewport)

    // TODO: we should have some check for if max screen size changes (to update viewport, should only happen if they change screen resolution? or use 2 monitors??)

    // NOTE: max := max(width, height), the user's larger screen dimension
    // This is an upper bound on the possible size of the viewport, so image does not need to change when the viewport resizes
    // This removes the jittering issue from before
    ImGui::Image((ImTextureID)viewport, ImVec2(max, max));
    UpdateViewport();

    ImGui::End();
    ImGui::PopStyleVar(); // Restore padding for other windows

    // TODO: dont allow hide tab bar, also stop highlighting it when it is clicked on
    // Draw all tabs that are open
    int i = 0;
    while(i < openTabs.size()){
        openTabs[i]->Begin();
        i++;
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
    // unique_ptr so will automatically delete 
    openTabs.clear(); // TODO: might not even need this tbh
    Audio::soloud.deinit();

    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyTexture(viewport);
    SDL_DestroyRenderer(renderer);
    TTF_Quit();
    // Mix_CloseAudio();    
    SDL_DestroyWindow(window);
    SDL_Quit();
}