#ifndef INPUT_H
#define INPUT_H

#include <SDL.h>

// Will be included in user scripts, indicated by "Salty" being part of name
class Input {
    public:
        static bool KeyDown[SDL_NUM_SCANCODES];
        static bool KeyUp[SDL_NUM_SCANCODES];
};

#endif // INPUT_H