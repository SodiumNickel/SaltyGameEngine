#ifndef ENGINEDATA_H
#define ENGINEDATA_H

#include <cstdint>
#include <string>

#include <SDL.h>


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

    // Only logs of at least logLevel will be considered
    int logLevel;

    // Engine Debug Data (only used in engine development)
    bool showEntityIds = false;
};

#endif // ENGINEDATA_H