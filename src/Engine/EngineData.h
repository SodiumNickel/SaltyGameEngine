#ifndef ENGINEDATA_H
#define ENGINEDATA_H

#include <cstdint>

struct EngineData {
    EngineData(int sceneIndex, uint64_t targetFrameTime) : sceneIndex(sceneIndex), targetFrameTime(targetFrameTime) {};
    
    int sceneIndex;
    // TODO: should i keep the scene name as a string too?

    // Target time per frame in ms
    uint64_t targetFrameTime;
    // Total time elapsed since engine opened in ms
    uint64_t prevFrameTime = 0;
};

#endif