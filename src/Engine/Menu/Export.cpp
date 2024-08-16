#include "Engine/Menu/Menu.h"

#include <cstdlib>
#include <fstream>

#include <imgui.h>
#include <imgui_stdlib.h>
#include <json.hpp>
using json = nlohmann::json;

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

// Adds user made scripts to UserScripts.h (includes and map)
void HandleUserScripts(){
    // Parse script data
    std::ifstream f("Unique/scripts.json");
    json jScripts = json::parse(f)["filepaths"];
    f.close();
    std::string inc = "";
    std::string map = "";
    for(int scriptIdx = 0; scriptIdx < jScripts.size(); scriptIdx++){
        std::string filepath = jScripts[scriptIdx].get<std::string>();
        inc += "#include \"" + filepath + ".h\"\n";
        // Assumes class name is given by name of file
        // TODO: add a check here that debug logs a proper error if not
        int lastSlashPos = filepath.find_last_of('/');
        std::string className = filepath;
        if (lastSlashPos != std::string::npos) className = filepath.substr(lastSlashPos + 1);

        map += "{\"" + filepath + "\", &CreateInstance<" + "PlayerMovement" + ">}";
        if(scriptIdx + 1 < jScripts.size()) map += ", ";
    }
    
    std::string userScripts1 =
"#ifndef USERSCRIPTS_H\n\
#define USERSCRIPTS_H\n\
\n\
// USER SCRIPT INCLUDES - written by engine\n";
    // Will place include here
    std::string userScripts2 =
"\n\
#include \"Game/ECS/ECS.h\"\n\
#include \"Game/Salty/SaltyTypes.h\"\n\
\n\
#include <map>\n\
#include <memory>\n\
#include <string>\n\
\n\
template<typename TScript> \n\
IScript* CreateInstance(Entity* entity, Transform* transform, std::vector<SaltyType>& serializedVars) { \n\
    return new TScript(entity, transform, serializedVars); \n\
}\n\
\n\
std::map<std::string, IScript*(*)(Entity*, Transform*, std::vector<SaltyType>&)> scriptMap = {\n\
// USER SCRIPT MAPPING - written by engine\n";
    // Will place mappings here
    std::string userScripts3 = 
"\n};\n\
\n\
#endif // USERSCRIPTS_H";

    std::ofstream g("Make/src/Game/UserScripts.h");
    g << userScripts1 + inc + userScripts2 + map + userScripts3;
    g.close();
}
// Removes user made scripts from UserScripts.h (includes and map)
void UnhandleUserScripts(){
    std::string userScripts = 
    "#ifndef USERSCRIPTS_H\n\
    #define USERSCRIPTS_H\n\
    \n\
    // USER SCRIPT INCLUDES - written by engine\n\
    \n\
    #include \"Game/ECS/ECS.h\"\n\
    #include \"Game/Salty/SaltyTypes.h\"\n\
    \n\
    #include <map>\n\
    #include <memory>\n\
    #include <string>\n\
    \n\
    template<typename TScript> \n\
    IScript* CreateInstance(Entity* entity, Transform* transform, std::vector<SaltyType>& serializedVars) { \n\
        return new TScript(entity, transform, serializedVars); \n\
    }\n\
    \n\
    std::map<std::string, IScript*(*)(Entity*, Transform*, std::vector<SaltyType>&)> scriptMap = {\n\
        // USER SCRIPT MAPPING - written by engine\n\
    };\n\
    \n\
    #endif // USERSCRIPTS_H";

    std::ofstream f("Make/src/Game/UserScripts.h");
    f << userScripts;
    f.close();
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
    
    // Paired with UnhandleUserScripts() below, links user made scripts to other source files
    HandleUserScripts();

    // Compile game into .exe
    std::string src = "Make/src/main.cpp Make/src/Game/Game.cpp Make/src/Game/ECS/ECS.cpp " 
                      "Make/src/Game/AssetManager/AssetManager.cpp Make/src/Game/Salty/SaltyDebug.cpp "
                      "Make/src/Game/Salty/SaltyInput.cpp Make/src/Game/Salty/SaltyAudio.cpp Make/src/Game/Salty/SaltyCamera.cpp ";
    // Add user made scripts into source files
    std::string usersrc = "";
    for(int scriptIdx = 0; scriptIdx < engineData->scriptFilepaths.size(); scriptIdx++){
        usersrc += "Unique/Assets/" + engineData->scriptFilepaths[scriptIdx] + ".cpp ";
    }
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

    UnhandleUserScripts();
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

    // Paired with UnhandleUserScripts() below, links user made scripts to other source files
    HandleUserScripts();

    // Compile game into .exe
    std::string src = "Make/src/webmain.cpp Make/src/Game/Game.cpp Make/src/Game/ECS/ECS.cpp " 
                      "Make/src/Game/AssetManager/AssetManager.cpp Make/src/Game/Salty/SaltyDebug.cpp "
                      "Make/src/Game/Salty/SaltyInput.cpp Make/src/Game/Salty/SaltyAudio.cpp Make/src/Game/Salty/SaltyCamera.cpp ";
    // Add user made scripts into source files
    std::string usersrc = "";
    for(int scriptIdx = 0; scriptIdx < engineData->scriptFilepaths.size(); scriptIdx++){
        usersrc += "Unique/Assets/" + engineData->scriptFilepaths[scriptIdx] + ".cpp ";
    }
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
    std::string flags =  "-DWITH_SDL2_STATIC -DWEBBUILD ";                        
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

    UnhandleUserScripts();
}