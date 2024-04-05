#include "AssetManager.h"

#include <SDL_image.h>
#include <glm.hpp>

#include <iostream>

AssetManager::AssetManager() {}

AssetManager::~AssetManager()
{
    ClearAssets();
}

void AssetManager::ClearAssets()
{
    for(auto texture : textures)
    { SDL_DestroyTexture(texture.second); }
    textures.clear();
}

void AssetManager::AddTexture(SDL_Renderer* renderer, const std::string& filepath)
{
    if(!textures.count(filepath))
    {
        // TODO: need to check that filepath is valid
        SDL_Surface* surface = IMG_Load(("Unique/Assets/" + filepath).c_str());
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);

        SDL_Point sdlSize;
        SDL_QueryTexture(texture, NULL, NULL, &sdlSize.x, &sdlSize.y);
        glm::vec2 size = glm::vec2(sdlSize.x, sdlSize.y);

        textures.emplace(filepath, texture);
        textureSizes.emplace(filepath, size);
    }
}

SDL_Texture* AssetManager::GetTexture(const std::string& filepath)
{
    // TODO: am assuming it is called on actual id, actually should be, because it is only called internally??
    return textures[filepath];
}

glm::vec2 AssetManager::GetTextureSize(const std::string& filepath)
{
    // TODO: am assuming it is called on actual id
    return textureSizes[filepath];
}