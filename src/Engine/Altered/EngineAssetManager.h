#ifndef ENGINEASSETMANAGER_H
#define ENGINEASSETMANAGER_H

#include <map>
#include <memory>
#include <string>

#include <SDL.h>
#include <SDL_ttf.h>
#include <glm.hpp>

#include "Engine/EngineData.h"

class EngineAssetManager {
private:
    SDL_Renderer* renderer;

    std::map<std::string, SDL_Texture*> textures; // TODO: could probably combine these two into one map, so we dont need to double access
    std::map<std::string, glm::vec2> textureSizes; // two
    // TODO: might have seperate font system later (with textures already made)
    std::map<std::string, TTF_Font*> fonts;

    std::shared_ptr<EngineData> engineData;
public:
    EngineAssetManager(SDL_Renderer* renderer, std::shared_ptr<EngineData> engineData)
    : renderer(renderer), engineData(engineData) {};
    ~EngineAssetManager();

    void ClearAssets();

    void AddTexture(const std::string& filepath);
    SDL_Texture* GetTexture(const std::string& filepath);
    glm::vec2 GetTextureSize(const std::string& filepath);

    void AddFont(const std::string& filepath, int fontSize);
    TTF_Font* GetFont(const std::string& filepath);
};

#endif // ENGINEASSETMANAGER_H