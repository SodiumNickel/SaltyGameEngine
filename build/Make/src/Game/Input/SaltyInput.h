#ifndef SALTYINPUT_H
#define SALTYINPUT_H

#include <SDL.h>

// Will be included in user scripts, indicated by "Salty" being part of name
class Input {
    public:
        // Keys that were pressed this frame
        static bool KeyDown[SDL_NUM_SCANCODES];
        // Keys that were pressed previously and not lifted
        static bool KeyHeld[SDL_NUM_SCANCODES];
        // Keys that were released this frame
        static bool KeyUp[SDL_NUM_SCANCODES];
};

#endif // SALTYINPUT_H