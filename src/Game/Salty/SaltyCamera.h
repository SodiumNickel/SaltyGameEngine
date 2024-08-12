#ifndef SALTYCAMERA_H
#define SALTYCAMERA_H
// Will be included in user scripts, indicated by "Salty" being part of name

#include <glm.hpp>

class Camera {
    static glm::vec2 position;
    static float rotation;

    static glm::vec2 dimensions;
};

#endif