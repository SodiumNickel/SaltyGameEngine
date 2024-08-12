#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include <algorithm>
#include <iostream>

#include <SDL.h>

#include "Game/ECS/ECS.h"
#include "Game/Components/TransformComponent.h"
#include "Game/Components/TextComponent.h"
#include "Game/AssetManager/AssetManager.h"


class RenderSystem : public System {
public:
    RenderSystem()
    {
        // RequireComponent<TransformComponent>();
        RequireComponent<TextComponent>();
    }

    // TODO: dont like this as a unique_ptr reference, or rather unsure if that is optimal?
    void Update(SDL_Renderer* renderer, std::unique_ptr<AssetManager>& assetManager, glm::vec2 cameraCenter)
    {
        // std::vector<Entity> entities = GetSystemEntities();

        // for(auto entity : entities)
        // {
        //     TransformComponent& transform = entity.GetComponent<TransformComponent>();
        //     const auto text = entity.GetComponent<TextComponent>();

        //     float cos = glm::cos(transform.rotation / 180 * 3.14);
        //     float sin = glm::sin(transform.rotation / 180 * 3.14);

        //     SDL_Rect dstRect = {
        //         static_cast<int>((transform.position.x  - cameraCenter.x)), 
        //         static_cast<int>(-(transform.position.y - cameraCenter.y)), // Negative so position y-axis points "up"
        //         static_cast<int>(textureSize.x * glm::abs(transform.scale.x)),
        //         static_cast<int>(textureSize.y * glm::abs(transform.scale.y))
        //     };

        //     // Handle negative scales by flipping sprite
        //     SDL_RendererFlip flip = SDL_FLIP_NONE;
        //     if(transform.scale.x < 0 || transform.scale.y < 0)
        //     {
        //         if(transform.scale.x >= 0) flip = SDL_FLIP_VERTICAL;
        //         else if(transform.scale.y >= 0) flip = SDL_FLIP_HORIZONTAL;
        //         else flip = static_cast<SDL_RendererFlip>(SDL_FLIP_VERTICAL | SDL_FLIP_HORIZONTAL); 
        //     }
            

        //     SDL_RenderCopyEx(
        //         renderer,
        //         assetManager->GetTexture(sprite.filepath),
        //         NULL, &dstRect, -transform.rotation, // rotations are counterclockwise
        //         NULL, flip
        //     );
        // }
    }
};

#endif // RENDERSYSTEM_H