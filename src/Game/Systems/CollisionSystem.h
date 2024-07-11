#ifndef COLLISIONSYSTEM_H
#define COLLISIONSYSTEM_H

#include <glm.hpp>

#include "Game/ECS/ECS.h"
#include "Game/Components/TransformComponent.h"
#include "Game/Components/BoxColliderComponent.h"
#include "Game/EventBus/EventBus.h"
#include "Game/Events/CollisionEvent.h"


class CollisionSystem : public System {
public:
    CollisionSystem()
    {
        RequireComponent<TransformComponent>();
        RequireComponent<BoxColliderComponent>();
    }

    void Update(std::unique_ptr<EventBus>& eventBus)
    {
        // TODO: add physics layers for collision
        // find some way to optimize this double loop, this is very ugly
        auto entities = GetSystemEntities();
        
        for(auto i = entities.begin(); i != entities.end(); i++)
        {
            Entity a = *i;
            auto aTransform = a.GetComponent<TransformComponent>();
            auto aCollider = a.GetComponent<BoxColliderComponent>();

            for(auto j = i+1; j != entities.end(); j++)
            {
                Entity b = *j;
                // Just in case we have multiple references to same entity
                if(a != b)
                {
                    auto bTransform = b.GetComponent<TransformComponent>();
                    auto bCollider = b.GetComponent<BoxColliderComponent>();

                    bool isCollision = CheckAABBCollision(
                        aTransform.position + aCollider.offset, 
                        aCollider.width * aTransform.scale.x, aCollider.height * aTransform.scale.y,
                        bTransform.position + bCollider.offset, 
                        bCollider.width * bTransform.scale.x, bCollider.height * bTransform.scale.y
                    );
                    if(isCollision)
                    {
                        //Logger::Log("Entity: " + std::to_string(a.GetId()) + " is colliding with entity " + std::to_string(b.GetId()));

                        eventBus->EmitEvent<CollisionEvent>(a, b);
                    }
                }
            }
        }
    }

private:// TODO: dont like how many variables are here
    bool CheckAABBCollision(glm::vec2 a, double aW, double aH, glm::vec2 b, double bW, double bH)
    {
        return (
            a.x < b.x + bW &&
            a.x + aW > b.x &&
            a.y < b.y + bH &&
            a.y + aH > b.y
        );
    }
};

#endif