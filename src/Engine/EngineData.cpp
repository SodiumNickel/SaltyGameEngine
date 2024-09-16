#include "EngineData.h"

#include <fstream>
#include <string>

#include <json.hpp>
using json = nlohmann::json;

EngineData::EngineData(){
    // Open most recently edited project
    std::ifstream f("EngineData/engine-settings.json");
    json jSettings = json::parse(f);
    f.close();

    projectName = jSettings["recent-project"].get<std::string>();
    assetsRootDir = "./Projects/" + projectName + "/Unique/Assets";
    // 1000 / fps = ms per frame
    targetFrameTime = 1000 / jSettings["target-fps"].get<int>();

    // Get engine data specific to recent project
    std::ifstream g("Projects/" + projectName + "/engine.json");
    json jEngineData = json::parse(g);
    g.close();

    // Get most recently open scene
    sceneIndex = jEngineData["recent-scene"].get<int>();
}