#ifndef BOXCOLLIDERCOMPONENT_H
#define BOXCOLLIDERCOMPONENT_H

#include <glm.hpp>

// 
struct SingleBoxCollider {
    int width;
    int height;
    glm::vec2 offset;
};

struct BoxColliderComponent {    
    SingleBoxCollider boxCols[1];

    // TODO: maybe store many structures with these things, and store a number of colliders
    // this way we can maintain the bitmap for component pool, but allow multiple box collider components
    // each one should have a label, will be useful in engine (like can label footCol)
    // TODO: this should probably scale off of the transform as well


    // TODO: maybe glm::vec2 scale?
    BoxColliderComponent(int width = 0, int height = 0, glm::vec2 offset = glm::vec2(0,0))
    {
        boxCols[0].width = width;
        boxCols[0].height = height;
        boxCols[0].offset = offset;
    }
};

#endif // BOXCOLLIDERCOMPONENT_H