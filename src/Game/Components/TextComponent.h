#ifndef TEXTCOMPONENT_H
#define TEXTCOMPONENT_H

#include <string>

#include <SDL.h>
#include <glm.hpp>

struct TextComponent {
    std::string filepath;
    std::string text;
    glm::vec2 offset;
    int zIndex;
    SDL_Color color;

    TextComponent(std::string filepath = "", std::string text = "", glm::vec2 offset = glm::vec2(0,0), int zIndex = 0, const SDL_Color& color = {0, 0, 0, 1})
    {
        this->filepath = filepath;
        this->text = text;
        this->offset = offset;
        this->zIndex = zIndex;
        this->color = color;
    }
};

#endif // TEXTCOMPONENT_H