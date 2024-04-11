#ifndef TRANSFORMCOMPONENT_H
#define TRANSFORMCOMPONENT_H

#include <glm.hpp>

#include "./InEngine.h"

struct TransformComponent {
    glm::vec2 position;
    glm::vec2 scale;
    float rotation;

    TransformComponent(glm::vec2 position = glm::vec2(0,0), glm::vec2 scale = glm::vec2(1, 1), float rotation = 0.0)
    {
        this->position = position;
        this->scale = scale;
        this->rotation = rotation;
    }

    #ifdef INENGINE
    template <typename T>
    void SetValue(ComponentVars compVar, T value){
        switch(compVar){
            case POSITION_X: position.x = value; break;
            case POSITION_Y: position.y = value; break;
            case SCALE_X: scale.x = value; break;
            case SCALE_Y: scale.y = value; break;
            case ROTATION: rotation = value; break;
            default: break; // TODO: some log
        }
    }
    #endif
};

#endif