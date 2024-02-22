#ifndef SPRITECOMPONENT_H
#define SPRITECOMPONENT_H

#include <string>

struct SpriteComponent {
    std::string filePath;
    int zIndex;

    SpriteComponent(std::string filePath = "", int zIndex = 0)
    {
        this->filePath = filePath;
        this->zIndex = zIndex;
    }
};

#endif