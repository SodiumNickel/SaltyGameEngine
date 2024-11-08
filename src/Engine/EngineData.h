#ifndef ENGINEDATA_H
#define ENGINEDATA_H

#include <cstdint>
#include <map>
#include <string>
#include <windows.h>

#include <SDL.h>

#include "Game/Salty/SaltyTypes.h"

struct ScriptData {
    std::string filepath;
    std::vector<std::string> varTypes;
    std::vector<std::string> varNames;
    std::vector<SaltyType> varValues;
};

struct EngineData {
    EngineData();
    // Target time per frame in ms (for engine)
    uint64_t targetFrameTime;
    // Total time elapsed since engine opened in ms
    uint64_t prevFrameTime = 0;

    std::string projectName;

    // Stores currently open scene (initialized to most recently open scene)
    int sceneIndex;
    std::string sceneName = "";

    int selectedEntity = -1; // TODO: should start at first thing in scene (i.e. rootIds[0])

    // Stores which scripts each entity in current scene has (and their serialized variable values)
    std::vector<std::vector<ScriptData>> scriptTree; // TODO: maybe we make this a ptr to ScriptData later
    std::vector<std::string> scriptFilepaths;
    std::vector<std::string> scriptNames;
    // Maps scriptFilepath to entityId
    std::map<std::string, std::vector<int>> scriptMap;
    std::vector <FILETIME> scriptEditTimes;

    std::string assetsRootDir = "";
    // Stores directory most recently accessed AssetTab was in 
    // When scripts are created they will be placed there
    // TODO: make sure we have a check for if the folder doesnt exist (will be handled by asset tab)
    std::string recentAssetDir = "";

    // For exporting game builds
    std::string gameName = "Game name";
    std::string gameFilepath = "";
    glm::ivec2 windowDimensions;

    // Only logs of at least logLevel will be considered
    int logLevel;

    // Engine Debug Data (only used in engine development)
    bool showEntityIds = false;
};

#endif // ENGINEDATA_H