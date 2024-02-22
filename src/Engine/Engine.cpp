#include "Engine.h"
#include "EnGUI.h"

#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include <SDL.h>
#include <SDL_image.h>

#include <iostream>

// Constructor
Engine::Engine()
{
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
    SDL_GetCurrentDisplayMode(0, &dm);
    auto width = dm.w;
    auto height = dm.h - 75; // adjusted for border size

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

    // Send renderer and viewport to stage
    stage.Initialize(renderer, viewport);

    // Init imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); 
    
    // Set ImGuiIO Flags
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
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
    }
}

void Engine::ProcessInput()
{
    SDL_Event e;
    while(SDL_PollEvent(&e)){
        if (e.type == SDL_QUIT){
            isRunning = false;
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
    ImGui::NewFrame();

    // Dockspace
    // Remove padding from dockspace
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace Demo", nullptr, dockspace_flags);
    
    ImGui::PopStyleVar(3);

    // Submit the DockSpace
    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

    // Top menu bar
    Menu();

    // Stage
    ImGui::Begin("Stage");
    
    ImVec2 stagePos = ImGui::GetWindowPos(); // TODO: feel like i'll need this later to keep viewport still, might have to be delta
    ImVec2 stageSize = ImGui::GetWindowSize();
    stageSize.x -= 16; // makes all borders equal size
    stageSize.y -= 35; // adjusted for tab bar, hides scroll

    stage.stageSize = stageSize;
    UpdateViewport();

    ImGui::Image((ImTextureID)viewport, stageSize);
    ImGui::End();

    // TODO: dont allow hide tab bar
    // TODO: do each of the different tabs here
    EntityTab();
    ComponentTab();
    ScriptTab();
    AssetTab();

    ImGui::ShowDemoWindow();

    // End dockspace
    ImGui::End();
}

void Engine::UpdateViewport()
{
    // TODO: the images get stretched by this amount soooo, they need to be scaled back based on ratio
    // just divide by w, h in each direction

    // TODO: should either call stage or game render system
    // create stage earlier, pass renderer, viewport, then they can do whatever

    // if in stage
    if(true){
        stage.Run();
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
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyTexture(viewport);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}