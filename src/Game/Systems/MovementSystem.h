#ifndef MOVEMENTSYSTEM_H
#define MOVEMENTSYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidbodyComponent.h"

// TODO: wanted to change the name of this, maybe just rigidbodysystem
class MovementSystem : public System {
    public:
        MovementSystem()
        {
            // RequireComponent<TransformComponent>();
            RequireComponent<RigidbodyComponent>();
        }

        void Update(float deltaTime)
        {
            for(auto entity : GetSystemEntities())
            {
                // TODO: make sure this works with new transform implementation
                // TODO: can also do TransformComponent& ..., whatever looks better
                auto& transform = *entity.transform;
                const auto rigidbody = entity.GetComponent<RigidbodyComponent>();

                transform.position.x += rigidbody.velocity.x * deltaTime;
                transform.position.y += rigidbody.velocity.y * deltaTime;

                //Logger::Log(std::to_string(transform.position.x) + ", " + std::to_string(transform.position.y));
            }
        }
};

#endif