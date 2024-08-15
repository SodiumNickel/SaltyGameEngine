#include "Engine/Menu/Menu.h"

#include <cstdlib>

#include <imgui.h>
#include <imgui_stdlib.h>

void Menu::ExportPopup(){
    if (ImGui::BeginPopupModal("Export"))
    {
        ImGui::InputText("##gamename", &engineData->gameName);
        ImGui::InputText("##gamefilepath", &engineData->gameFilepath);

        if (ImGui::Button("Windows Export")) ExportWindows();
        if (ImGui::Button("Web Export")) ExportWeb();
 
        if (ImGui::Button("Close")) ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }
}

void Menu::ExportWindows(){
    // TODO: do the thing where we loop through (1), (2) until we find a valid path
    // Create directory for game inside of gameFilepath 
    std::string dir = "mkdir \"" + engineData->gameFilepath + "\"\\\"" + engineData->gameName + "\"";
    int result = system(dir.c_str());
    if (result == 0) {
        std::cout << "Created dir successful!\n";
    } else {
        std::cerr << "Created dir failed!\n";
    }

    // Compile game into .exe
    std::string src = "Make/src/main.cpp Make/src/Game/Game.cpp Make/src/Game/ECS/ECS.cpp " 
                      "Make/src/Game/AssetManager/AssetManager.cpp Make/src/Game/Helpers/JsonHelper.cpp "
                      "Make/src/Game/Salty/SaltyInput.cpp Make/src/Game/Salty/SaltyAudio.cpp Make/src/Game/Salty/SaltyCamera.cpp ";
    // TODO: EDITED BY ENGINE, SHOULD just be a variable tbh
    std::string usersrc = "Unique/Assets/PlayerMovement.cpp ";
    // TODO: statically link soloud instead, or acatually.... might be fine
    std::string soloudcore = "Make/libsrc/soloud/core/soloud.cpp Make/libsrc/soloud/core/soloud_audiosource.cpp"
                         " Make/libsrc/soloud/core/soloud_bus.cpp Make/libsrc/soloud/core/soloud_core_3d.cpp"
                         " Make/libsrc/soloud/core/soloud_core_basicops.cpp Make/libsrc/soloud/core/soloud_core_faderops.cpp"
                         " Make/libsrc/soloud/core/soloud_core_filterops.cpp Make/libsrc/soloud/core/soloud_core_getters.cpp"
                         " Make/libsrc/soloud/core/soloud_core_setters.cpp Make/libsrc/soloud/core/soloud_core_voicegroup.cpp"
                         " Make/libsrc/soloud/core/soloud_core_voiceops.cpp Make/libsrc/soloud/core/soloud_fader.cpp"
                         " Make/libsrc/soloud/core/soloud_fft.cpp Make/libsrc/soloud/core/soloud_fft_lut.cpp"
                         " Make/libsrc/soloud/core/soloud_file.cpp Make/libsrc/soloud/core/soloud_filter.cpp"
                         " Make/libsrc/soloud/core/soloud_misc.cpp Make/libsrc/soloud/core/soloud_queue.cpp"
                         " Make/libsrc/soloud/core/soloud_thread.cpp ";
    std::string soloudother = "Make/libsrc/soloud/sdl2_static/soloud_sdl2_static.cpp Make/libsrc/soloud/wav/dr_impl.cpp"
                              " Make/libsrc/soloud/wav/soloud_wav.cpp Make/libsrc/soloud/wav/soloud_wavstream.cpp "
                              " Make/libsrc/soloud/wav/stb_vorbis.c ";
    std::string flags =  "-DWITH_SDL2_STATIC ";                        
    // END TODO
    std::string inc = "-IMake/include/SDL2 -IMake/include/glm -IMake/include/nlohmann -IMake/include/soloud -IMake/libsrc/soloud/wav -IMake/src -IUnique/Assets ";
    std::string lib = "-LMake/lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf ";
    std::string out = "-o \"" + engineData->gameFilepath + "\"/\"" + engineData->gameName + "\"/\"" + engineData->gameName + ".exe\"";

    // TODO: do need a way to determine that user has proper packages installed (is it just mingw here?)
    std::string make = "g++ " + src + usersrc + soloudcore + soloudother + "-std=c++17 " + inc + lib + flags + out;
    result = system(make.c_str());
    // TODO: v also want to rename result
    // TEMP
    if (result == 0) {
        std::cout << "Compilation successful!\n";
    } else {
        std::cerr << "Compilation failed!\n";
    }

    // Copy Unique folder
    // TODO: I would like to remove the printed console stuff
    std::string copy = "xcopy /E /I /Y .\\Unique\\ \"" + engineData->gameFilepath + "\"\\\"" + engineData->gameName + "\"\\Unique";
    result = system(copy.c_str());
    if (result == 0) {
        std::cout << "Copy unique successful!\n";
    } else {
        std::cerr << "Copy unique failed!\n";
    }

    // Copy dll's
    copy = "xcopy /I /Y .\\*.dll \"" + engineData->gameFilepath + "\"\\\"" + engineData->gameName + "\"";
    result = system(copy.c_str());
    if (result == 0) {
        std::cout << "Copy dll's successful!\n";
    } else {
        std::cerr << "Copy dll's failed!\n";
    }
}

void Menu::ExportWeb(){
    // TODO: do the thing where we loop through (1), (2) until we find a valid path
    // Create directory for game inside of gameFilepath 
    std::string dir = "mkdir \"" + engineData->gameFilepath + "\"\\\"" + engineData->gameName + "\"";
    int result = system(dir.c_str());
    if (result == 0) {
        std::cout << "Created dir successful!\n";
    } else {
        std::cerr << "Created dir failed!\n";
    }

    // Compile game into .exe
    std::string src = "Make/src/webmain.cpp Make/src/Game/Game.cpp Make/src/Game/ECS/ECS.cpp " 
                      "Make/src/Game/AssetManager/AssetManager.cpp Make/src/Game/Helpers/JsonHelper.cpp "
                      "Make/src/Game/Salty/SaltyInput.cpp Make/src/Game/Salty/SaltyAudio.cpp Make/src/Game/Salty/SaltyCamera.cpp ";
    // TODO: EDITED BY ENGINE, SHOULD just be a variable tbh
    std::string usersrc = "Unique/Assets/PlayerMovement.cpp ";
    // TODO: statically link soloud instead
    std::string soloudcore = "Make/libsrc/soloud/core/soloud.cpp Make/libsrc/soloud/core/soloud_audiosource.cpp"
                         " Make/libsrc/soloud/core/soloud_bus.cpp Make/libsrc/soloud/core/soloud_core_3d.cpp"
                         " Make/libsrc/soloud/core/soloud_core_basicops.cpp Make/libsrc/soloud/core/soloud_core_faderops.cpp"
                         " Make/libsrc/soloud/core/soloud_core_filterops.cpp Make/libsrc/soloud/core/soloud_core_getters.cpp"
                         " Make/libsrc/soloud/core/soloud_core_setters.cpp Make/libsrc/soloud/core/soloud_core_voicegroup.cpp"
                         " Make/libsrc/soloud/core/soloud_core_voiceops.cpp Make/libsrc/soloud/core/soloud_fader.cpp"
                         " Make/libsrc/soloud/core/soloud_fft.cpp Make/libsrc/soloud/core/soloud_fft_lut.cpp"
                         " Make/libsrc/soloud/core/soloud_file.cpp Make/libsrc/soloud/core/soloud_filter.cpp"
                         " Make/libsrc/soloud/core/soloud_misc.cpp Make/libsrc/soloud/core/soloud_queue.cpp"
                         " Make/libsrc/soloud/core/soloud_thread.cpp ";
    std::string soloudother = "Make/libsrc/soloud/sdl2_static/soloud_sdl2_static.cpp Make/libsrc/soloud/wav/dr_impl.cpp"
                              " Make/libsrc/soloud/wav/soloud_wav.cpp Make/libsrc/soloud/wav/soloud_wavstream.cpp "
                              " Make/libsrc/soloud/wav/stb_vorbis.c ";
    std::string flags =  "-DWITH_SDL2_STATIC ";                        
    // END TODO
    std::string inc = "-IMake/include/SDL2 -IMake/include/glm -IMake/include/nlohmann -Iinclude/emscripten -IMake/include/soloud -IMake/libsrc/soloud/wav -IMake/src -IUnique/Assets ";
    std::string wflags = "-s WASM=1 -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s USE_SDL_TTF=2 -s ALLOW_MEMORY_GROWTH=1 ";
    std::string preload = "--preload-file .\\Unique@Unique --use-preload-plugins ";
    std::string out = "-o \"" + engineData->gameFilepath + "\"/\"" + engineData->gameName + "\"/index.html";

    // TODO: do need a way to determine that user has proper packages installed (is it just mingw here?)
    std::string make = "em++ " + src + usersrc + soloudcore + soloudother + "-std=c++17 " + inc + wflags + flags + preload + out;
    result = system(make.c_str());
    // TODO: v also want to rename result
    // TEMP
    if (result == 0) {
        std::cout << "Compilation successful!\n";
    } else {
        std::cerr << "Compilation failed!\n";
    }

    // Overwrite index.html - assuming we are building for itch.io so includes added window focus
    std::string copy = "xcopy /I /Y .\\Make\\index.html \"" + engineData->gameFilepath + "\"\\\"" + engineData->gameName + "\"";
    result = system(copy.c_str());
    if (result == 0) {
        std::cout << "Copy index successful!\n";
    } else {
        std::cerr << "Copy index failed!\n";
    }
}