#ifndef ENGINEDATA_H
#define ENGINEDATA_H

struct EngineData {
    EngineData(int sceneIndex, Uint64 targetFrameTime) : sceneIndex(sceneIndex), targetFrameTime(targetFrameTime) {};
    
    int sceneIndex;
    // TODO: should i keep the scene name as a string too?

    // Target time per frame in ms
    Uint64 targetFrameTime;
    // Total time elapsed since engine opened in ms
    Uint64 prevFrameTime = 0;
};

#endif