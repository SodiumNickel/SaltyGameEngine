#ifndef SALTYCAMERA_H
#define SALTYCAMERA_H
// Will be included in user scripts, indicated by "Salty" being part of name

#include <glm.hpp>

class Camera {
public:
    static glm::vec2 position;
    static float rotation;

    // NOTE: Do not edit aspectRatio during runtime (TODO: for now...)
    static glm::ivec2 aspectRatio;
    // The amount the aspect ratio is scaled by
    static float scale; 
};

#endif