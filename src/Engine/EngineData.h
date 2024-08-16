#ifndef ENGINEDATA_H
#define ENGINEDATA_H

#include <cstdint>
#include <string>

#include <SDL.h>

#include "Game/Salty/SaltyTypes.h"

struct ScriptData {
    std::string filepath;
    std::vector<std::string> varTypes;
    std::vector<std::string> varNames;
    std::vector<SaltyType> varValues;
};

struct EngineData {
    EngineData(int sceneIndex, uint64_t targetFrameTime) : sceneIndex(sceneIndex), targetFrameTime(targetFrameTime) {};
    // Target time per frame in ms
    uint64_t targetFrameTime;
    // Total time elapsed since engine opened in ms
    uint64_t prevFrameTime = 0;

    // TODO: comments here, also reorder initialization vars to match this
    int sceneIndex;
    std::string sceneName = "";

    int selectedEntity = -1; // TODO: should start at first thing in scene (i.e. rootIds[0])

    // Stores which scripts each entity in current scene has (and their serialized variable values)
    std::vector<std::vector<ScriptData>> scriptTree; // TODO: maybe we make this a ptr to ScriptData later

    // For exporting game builds
    std::string gameName = "Game name";
    std::string gameFilepath = "";

    // Only logs of at least logLevel will be considered
    int logLevel;

    // Engine Debug Data (only used in engine development)
    bool showEntityIds = false;
};

#endif // ENGINEDATA_H