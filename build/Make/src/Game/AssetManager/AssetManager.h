#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H

#include <map>
#include <string>

#include <SDL.h>
#include <glm.hpp>


class AssetManager {
private:
    std::map<std::string, SDL_Texture*> textures;
    std::map<std::string, glm::vec2> textureSizes;
    // TODO: create map for fonts, audio, ...

public:
    AssetManager();
    ~AssetManager();

    void ClearAssets();
    void AddTexture(SDL_Renderer* renderer, const std::string& filepath);
    SDL_Texture* GetTexture(const std::string& filepath);
    glm::vec2 GetTextureSize(const std::string& filepath);
};

#endif // ASSETMANAGER_H