#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include <algorithm>
#include <iostream>

#include <SDL.h>

#include "Game/ECS/ECS.h"
#include "Game/Components/TransformComponent.h"
#include "Game/Components/SpriteComponent.h"
#include "Game/AssetManager/AssetManager.h"


class RenderSystem : public System {
public:
    RenderSystem()
    {
        // RequireComponent<TransformComponent>();
        RequireComponent<SpriteComponent>();
    }

    // TODO: should this be a unique_ptr for assetmanager in game?
    void Update(SDL_Renderer* renderer, std::shared_ptr<AssetManager> assetManager, glm::vec2 cameraCenter, glm::vec2 cameraZoom)
    {
        // TODO: optimize by sorting sprite objects whenever they are added
        // Can do this with a simple insertion on frames with low entity additions
        // And with quicksort (or similar) for ones with more
        std::vector<Entity> entities = GetSystemEntities();
        // TODO: im worried that if objects have same layer, they will swap whos in front
        // Could add a secondary sort value by entityId, higher is above
        std::sort(entities.begin(), entities.end(), [](const Entity& a, const Entity& b)
            { return a.GetComponent<SpriteComponent>().zIndex
            < b.GetComponent<SpriteComponent>().zIndex; });

        for(auto entity : entities)
        {
            TransformComponent& transform = entity.GetComponent<TransformComponent>();
            const auto sprite = entity.GetComponent<SpriteComponent>();
            glm::vec2 textureSize = assetManager->GetTextureSize(sprite.filepath);

            float cos = glm::cos(transform.rotation / 180 * 3.14);
            float sin = glm::sin(transform.rotation / 180 * 3.14);
            glm::vec2 camScale = glm::normalize(glm::vec2(glm::abs(cameraZoom.x * cos) + glm::abs(cameraZoom.y * sin),
                                                          glm::abs(cameraZoom.x * sin) + glm::abs(cameraZoom.y * cos)));
            float magnitude = glm::length(cameraZoom);

            SDL_Rect dstRect = {
                static_cast<int>((transform.position.x  - cameraCenter.x) * cameraZoom.x), // TODO: i dont think this scales properly off of center of object, scales off of corner
                static_cast<int>(-(transform.position.y - cameraCenter.y) * cameraZoom.y), // Negative so position y-axis points "up"
                static_cast<int>(textureSize.x * glm::abs(transform.scale.x) * camScale.x * magnitude),
                static_cast<int>(textureSize.y * glm::abs(transform.scale.y) * camScale.y * magnitude)
            };

            // Handle negative scales by flipping sprite
            SDL_RendererFlip flip = SDL_FLIP_NONE;
            if(transform.scale.x < 0 || transform.scale.y < 0)
            {
                if(transform.scale.x >= 0) flip = SDL_FLIP_VERTICAL;
                else if(transform.scale.y >= 0) flip = SDL_FLIP_HORIZONTAL;
                else flip = static_cast<SDL_RendererFlip>(SDL_FLIP_VERTICAL | SDL_FLIP_HORIZONTAL); 
            }
            

            SDL_RenderCopyEx(
                renderer,
                assetManager->GetTexture(sprite.filepath),
                NULL, &dstRect, -transform.rotation, // rotations are counterclockwise
                NULL, flip
            );
        }
    }
};

#endif // RENDERSYSTEM_H