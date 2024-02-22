#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include <SDL.h>

#include "../ECS/ECS.h"
#include "../AssetManager/AssetManager.h"

#include "../Components/TransformComponent.h"
#include "../Components/SpriteComponent.h"

#include <algorithm>

#include <iostream>

class RenderSystem : public System {
public:
    RenderSystem()
    {
        RequireComponent<TransformComponent>();
        RequireComponent<SpriteComponent>();
    }

    void Update(SDL_Renderer* renderer, std::unique_ptr<AssetManager>& assetManager, glm::vec2 cameraZoom)
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
            const auto transform = entity.GetComponent<TransformComponent>();
            const auto sprite = entity.GetComponent<SpriteComponent>();
            glm::vec2 textureSize = assetManager->GetTextureSize(sprite.filePath);

            SDL_Rect dstRect = {
                static_cast<int>(transform.position.x * cameraZoom.x),
                static_cast<int>(transform.position.y * cameraZoom.y),
                static_cast<int>(textureSize.x * transform.scale.x * cameraZoom.x),
                static_cast<int>(textureSize.y * transform.scale.y * cameraZoom.y)
            };

            SDL_RenderCopyEx(
                renderer,
                assetManager->GetTexture(sprite.filePath),
                NULL, &dstRect, transform.rotation,
                NULL, SDL_FLIP_NONE
            );
        }
    }
};

#endif