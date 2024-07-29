#ifndef PHYSICSSYSTEM_H
#define PHYSICSSYSTEM_H

#include "Game/ECS/ECS.h"
#include "Game/Components/TransformComponent.h"
#include "Game/Components/RigidbodyComponent.h"


// TODO: wanted to change the name of this, maybe just rigidbodysystem
class PhysicsSystem : public System {
    public:
        PhysicsSystem()
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
                TransformComponent& transform = entity.GetComponent<TransformComponent>();
                const auto rigidbody = entity.GetComponent<RigidbodyComponent>();

                transform.position.x += rigidbody.velocity.x * deltaTime;
                transform.position.y += rigidbody.velocity.y * deltaTime;

                //Logger::Log(std::to_string(transform.position.x) + ", " + std::to_string(transform.position.y));
            }
        }
};

#endif // PHYSICSSYSTEM_H