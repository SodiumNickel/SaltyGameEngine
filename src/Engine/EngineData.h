#ifndef ENGINEDATA_H
#define ENGINEDATA_H

struct EngineData {
    EngineData(int sceneIndex) : sceneIndex(sceneIndex) {};
    
    int sceneIndex;
    // TODO: should i keep the scene name as a string too?
};

#endif