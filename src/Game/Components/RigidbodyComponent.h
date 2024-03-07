#ifndef RIGIDBODYCOMPONENT_H
#define RIGIDBODYCOMPONENT_H

#include <glm.hpp>

struct RigidbodyComponent {
    glm::vec2 velocity;
    // TODO: needs to have transform before adding

    RigidbodyComponent(glm::vec2 velocity = glm::vec2(0.0, 0.0))
    {
        this->velocity = velocity;
    }
};

#endif