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
        ImGui::Text("Name");
        ImGui::InputText("##gamename", &engineData->gameName);
        ImGui::Text("Filepath");
        ImGui::InputText("##gamefilepath", &engineData->gameFilepath);
        ImGui::SeparatorText("");

        ImGui::Text("Window Dimensions");
        ImGui::SetItemTooltip("\"Viewport Dimensions\" in itch.io");
        ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.4f);
        ImGui::Text("x"); ImGui::SameLine();
        ImGui::InputInt("##windowdimx", &engineData->windowDimensions.x); ImGui::SameLine();
        ImGui::Text("y"); ImGui::SameLine();
        ImGui::InputInt("##windowdimy", &engineData->windowDimensions.y);
        ImGui::PopItemWidth();

        ImGui::SeparatorText("");

        if (ImGui::Button("Windows Export")) ExportWindows();
        ImGui::SameLine();
        if (ImGui::Button("Web Export")) ExportWeb();

        if (ImGui::Button("Close")) ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }
}

// Adds user made scripts to UserScripts.cpp (includes and map)
void HandleUserScripts(std::string& currentProjectFilepath){
    // Parse script data
    std::ifstream f(currentProjectFilepath + "/Unique/scripts.json");
    json jScripts = json::parse(f);
    f.close();
    std::string inc = "";
    std::string map = "";
    std::string con = "";
    for(int scriptIdx = 0; scriptIdx < jScripts["filepaths"].size(); scriptIdx++){
        std::string filepath = jScripts["filepaths"][scriptIdx].get<std::string>();
        inc += "#include \"" + filepath + ".h\"\n";
        // Assumes class name is given by name of file
        // TODO: add a check here that debug logs a proper error if not
        int lastSlashPos = filepath.find_last_of('/');
        std::string className = filepath;
        if (lastSlashPos != std::string::npos) className = filepath.substr(lastSlashPos + 1);

        map += "{\"" + filepath + "\", &CreateInstance<" + className + ">}";
        if(scriptIdx + 1 < jScripts["filepaths"].size()) map += ", ";

        con += className + "::" + className + "(Entity* entity, Transform* transform, std::vector<SaltyType>& serializedVars)\n";
        con += ": IScript(entity, transform), \n";
        // varName(std::get<varType>(serializedVars[i])), 
        json jNames = jScripts[filepath]["names"];
        json jTypes = jScripts[filepath]["types"];
        for(int argIdx = 0; argIdx < jNames.size(); argIdx++){
            std::string type = jTypes[argIdx];
            if(type == "Entity" || type == "Transform" || type == "Sprite" || type == "Rigidbody") type += "*";
            con += jNames[argIdx].get<std::string>() + "(std::get<" + type + ">(serializedVars[" + std::to_string(argIdx) + "]))";
            if(argIdx + 1 < jNames.size()) con += ", ";
        }
        con += "{};\n\n";
    }
    // PlayerMovement::PlayerMovement(Entity* entity, Transform* transform, std::vector<SaltyType>& serializedVars)
    // : IScript(entity, transform), 
    // val(std::get<int>(serializedVars[0])), player(std::get<Transform*>(serializedVars[1])) // THIS IS THE PART I HAVE TO ADD
    // {};
    
    std::string userScripts1 =
"// USER SCRIPT INCLUDES - written by engine\n";
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
// USER SCRIPT CONSTRUCTORS - written by engine\n";

    std::ofstream g("Make/src/Game/UserScripts.cpp");
    g << userScripts1 + inc + userScripts2 + map + userScripts3 + con;
    g.close();
}
// Removes user made scripts from UserScripts.cpp (includes and map)
void UnhandleUserScripts(){
    std::string userScripts = 
"// USER SCRIPT INCLUDES - written by engine\n\
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
// USER SCRIPT CONSTRUCTORS - written by engine";

    std::ofstream f("Make/src/Game/UserScripts.cpp");
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
    HandleUserScripts(engineData->currentProjectFilepath);

    // Compile game into .exe
    std::string src = "Make/src/main.cpp Make/src/Game/Game.cpp Make/src/Game/ECS/ECS.cpp " 
                      "Make/src/Game/AssetManager/AssetManager.cpp Make/src/Game/Salty/SaltyDebug.cpp "
                      "Make/src/Game/Salty/SaltyInput.cpp Make/src/Game/Salty/SaltyAudio.cpp Make/src/Game/Salty/SaltyCamera.cpp ";
    // Add user made scripts into source files
    std::string usersrc = "";
    for(int scriptIdx = 0; scriptIdx < engineData->scriptFilepaths.size(); scriptIdx++){
        usersrc += engineData->currentProjectFilepath + "/Unique/Assets/" + engineData->scriptFilepaths[scriptIdx] + ".cpp ";
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
    std::string flags =  "-DWITH_SDL2_STATIC -DGAME_BUILD ";                        
    // END TODO
    std::string inc = "-IMake/include/SDL2 -IMake/include/glm -IMake/include/nlohmann -IMake/include/soloud -IMake/libsrc/soloud/wav -IMake/src ";
    inc += "-I\"" + engineData->currentProjectFilepath + "\"/Unique/Assets ";
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
    HandleUserScripts(engineData->currentProjectFilepath);

    // Compile game into .exe
    std::string src = "Make/src/webmain.cpp Make/src/Game/Game.cpp Make/src/Game/ECS/ECS.cpp " 
                      "Make/src/Game/AssetManager/AssetManager.cpp Make/src/Game/Salty/SaltyDebug.cpp "
                      "Make/src/Game/Salty/SaltyInput.cpp Make/src/Game/Salty/SaltyAudio.cpp Make/src/Game/Salty/SaltyCamera.cpp ";
    // Add user made scripts into source files
    std::string usersrc = "";
    for(int scriptIdx = 0; scriptIdx < engineData->scriptFilepaths.size(); scriptIdx++){
        usersrc += engineData->currentProjectFilepath + "/Unique/Assets/" + engineData->scriptFilepaths[scriptIdx] + ".cpp ";
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
    std::string flags =  "-DWITH_SDL2_STATIC -DGAME_BUILD -DWEB_BUILD ";                        
    // END TODO
    std::string inc = "-IMake/include/SDL2 -IMake/include/glm -IMake/include/nlohmann -Iinclude/emscripten -IMake/include/soloud -IMake/libsrc/soloud/wav -IMake/src ";
    inc += "-I\"" + engineData->currentProjectFilepath + "\"/Unique/Assets ";
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