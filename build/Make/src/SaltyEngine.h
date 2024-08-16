#ifndef SALTYENGINE_H
#define SALTYENGINE_H

// All includes relevant to user scripting

#include <variant>
#include <vector>

#include <SDL.h> // TODO: this is only used for SDL_Scancode right now, will probably remove later
#include <glm.hpp>

#include "Game/ECS/ECS.h"
#include "Game/Salty/SaltyAudio.h"
#include "Game/Salty/SaltyCamera.h"
#include "Game/Salty/SaltyDebug.h"
#include "Game/Salty/SaltyInput.h"
#include "Game/Salty/SaltyTypes.h"

// TODO: think about
// TODO: dont think LOCAL is the best name for this, but will use a similar method to keep vars not serialized
// Used to keep out of script tab in engine, and wont be initialized
#define LOCAL_ // TODO: want some differentiating thing, so it cant be a type, i dont like the _ but maybe somethign?
#define SF_ // Think i will stick with the _ after the tag
#define SF
#define _SF
#define SLIDER(min, max) // Except for ones with arguments? maybe just do this one in engine?
// Should also have ones for like headers and stuff for making engine organization easier

#endif