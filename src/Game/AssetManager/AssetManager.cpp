#include "Game/AssetManager/AssetManager.h"

#include <iostream>

#include <SDL_image.h>
#include <SDL_ttf.h>
#include <glm.hpp>

// TODO: asset manager should have access to renderer, this is also why textures will not render
AssetManager::AssetManager() {}

AssetManager::~AssetManager() { ClearAssets(); }

void AssetManager::ClearAssets()
{
    for(auto texture : textures)
    { SDL_DestroyTexture(texture.second); }
    textures.clear();

    for(auto font : fonts)
    { TTF_CloseFont(font.second); }
    fonts.clear();
}

void AssetManager::AddTexture(SDL_Renderer* renderer, const std::string& filepath)
{
    if(!textures.count(filepath))
    {
        // TODO: need to check that filepath is valid, just log some error here, should never happen but who knows
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
    // TODO: add this back once i add drag and drop (also count out default "")
    // assert(textures.count(filepath));
    return textures[filepath];
}

glm::vec2 AssetManager::GetTextureSize(const std::string& filepath)
{
    // assert(textureSizes.count(filepath));
    return textureSizes[filepath];
}


void AssetManager::AddFont(const std::string& filepath, int fontSize){
    fonts.emplace(filepath, TTF_OpenFont(("Unique/Assets/" + filepath).c_str(), fontSize));
}

TTF_Font* AssetManager::GetFont(const std::string& filepath){
    // assert(fonts.count(filepath));
    return fonts[filepath];
}