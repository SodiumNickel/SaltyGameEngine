#ifndef SPRITECOMPONENT_H
#define SPRITECOMPONENT_H

#include <string>


struct SpriteComponent {
    std::string filepath;
    int zIndex;

    SpriteComponent(std::string filepath = "", int zIndex = 0)
    {
        this->filepath = filepath;
        this->zIndex = zIndex;
    }
};

#endif // SPRITECOMPONENT_H