#include "Game/Salty/SaltyInput.h"

// Initialize input arrays
bool Input::KeyDown[SDL_NUM_SCANCODES] = {0};
bool Input::KeyHeld[SDL_NUM_SCANCODES] = {0};
bool Input::KeyUp[SDL_NUM_SCANCODES] = {0};
bool Input::MouseDown[NUM_M] = {0};
bool Input::MouseHeld[NUM_M] = {0};
bool Input::MouseUp[NUM_M] = {0};
int Input::MouseX = 0;
int Input::MouseY = 0;
