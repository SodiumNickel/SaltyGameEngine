#include "Game/Input/SaltyInput.h"

// Initialize input arrays
bool Input::KeyDown[SDL_NUM_SCANCODES] = {0};
bool Input::KeyHeld[SDL_NUM_SCANCODES] = {0};
bool Input::KeyUp[SDL_NUM_SCANCODES] = {0};