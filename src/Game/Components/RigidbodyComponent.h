#ifndef RIGIDBODYCOMPONENT_H
#define RIGIDBODYCOMPONENT_H

#include <glm.hpp>

struct RigidbodyComponent {
    glm::vec2 initVelocity;
    // TODO: needs to have transform before adding

    glm::vec2 velocity;

    RigidbodyComponent(glm::vec2 initVelocity = glm::vec2(0.0, 0.0))
    {
        this->initVelocity = initVelocity;
        this->velocity = initVelocity;
    }
};

#endif