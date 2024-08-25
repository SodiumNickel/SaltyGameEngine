#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include <algorithm>
#include <iostream>

#include <SDL.h>

#include "Game/ECS/ECS.h"
#include "Game/Components/TransformComponent.h"
#include "Game/Components/SpriteComponent.h"
#include "Game/AssetManager/AssetManager.h"
#include "Game/Salty/SaltyCamera.h"

class RenderSystem : public System {
public:
    RenderSystem()
    {
        // RequireComponent<TransformComponent>();
        RequireComponent<SpriteComponent>();
    }

    // TODO: dont like this as a unique_ptr reference, or rather unsure if that is optimal?
    void Update(SDL_Renderer* renderer, std::unique_ptr<AssetManager>& assetManager, int viewportScale)
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

        // Camera values
        glm::vec2 cameraCenter = Camera::position; // TODO: later position can be actual center, and can alter here
        float scale = viewportScale / Camera::scale;  // TODO: assertion that Camera::scale is not 0??

        for(auto entity : entities)
        {
            TransformComponent& transform = entity.GetComponent<TransformComponent>();
            const auto sprite = entity.GetComponent<SpriteComponent>();
            glm::vec2 textureSize = assetManager->GetTextureSize(sprite.filepath);

            float cos = glm::cos(transform.rotation / 180 * 3.14);
            float sin = glm::sin(transform.rotation / 180 * 3.14);

            SDL_Rect dstRect = {
                static_cast<int>((transform.position.x  - cameraCenter.x) * scale), 
                static_cast<int>(-(transform.position.y - cameraCenter.y) * scale), // Negative so position y-axis points "up"
                static_cast<int>(textureSize.x * glm::abs(transform.scale.x) * scale),
                static_cast<int>(textureSize.y * glm::abs(transform.scale.y) * scale)
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