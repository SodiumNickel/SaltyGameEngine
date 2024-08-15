#ifndef SALTYTYPES_H
#define SALTYTYPES_H

#include <string>
typedef std::string string;
#include <map> // TODO: should this be unordered?
#include <variant>

// TODO: should still group these into a seperate file (that contains all components) actually should i????
#include "Game/Components/TransformComponent.h"
typedef TransformComponent Transform;
#include "Game/Components/SpriteComponent.h"
typedef SpriteComponent Sprite;
#include "Game/Components/RigidbodyComponent.h"
typedef RigidbodyComponent Rigidbody;

#include "Game/ECS/ECS.h" // For Entity

#include "Game/Salty/SaltyAudio.h"

typedef std::variant
        <
            int, float, string,
            Entity*, Transform*, Sprite*, Rigidbody*,
            Sound
        >
        SaltyType;

#endif