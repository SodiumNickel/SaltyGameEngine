#ifndef SCRIPT_H
#define SCRIPT_H
#include "Game/ECS/ECS.h" // TODO: put into salty engine, also put all components
#include "Game/Components/TransformComponent.h"

class IScript {
protected:
    // TODO: Do i store owner entity and transform here?
    Entity* entity;
    TransformComponent* transform;
public:
    virtual void Start() = 0; 
    virtual void Update(float deltaTime) = 0;     
};

// TODO: this will be altered by engine to initialize script objects

#endif