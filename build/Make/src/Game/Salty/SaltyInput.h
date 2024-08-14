#ifndef SALTYINPUT_H
#define SALTYINPUT_H
// Will be included in user scripts, indicated by "Salty" being part of name

#include <SDL.h>

// TODO: kinda wanna move this to another file...
enum EMouseButtons {
    M_LEFT = 1,
    M_RIGHT = 2, // NOTE: I think having M2 (right click) is more standard, goes against SDL codes
    M_MID = 3,
    M_FOUR = 4,
    M_FIVE = 5,
    NUM_M = 8 // TODO: have this at power of 2 rn, also unsure about names
};

class Input {
    public:
        // Keys that were pressed this frame
        static bool KeyDown[SDL_NUM_SCANCODES];
        // Keys that were pressed previously and not lifted
        static bool KeyHeld[SDL_NUM_SCANCODES];
        // Keys that were released this frame
        static bool KeyUp[SDL_NUM_SCANCODES];


        // Mouse buttons that were pressed this frame
        static bool MouseDown[NUM_M];
        // Mouse buttons that were pressed previously and not lifted
        static bool MouseHeld[NUM_M];
        // Mouse buttons that were released this frame
        static bool MouseUp[NUM_M];
        // TODO: i want these to be scaled to world position, need to do some math
        static int MouseX;
        static int MouseY;
        
};

#endif // SALTYINPUT_H