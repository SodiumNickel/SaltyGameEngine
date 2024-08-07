#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H

#include <map>
#include <string>

#include <SDL.h>
#include <SDL_ttf.h>
#include <glm.hpp>


class AssetManager {
private:
    std::map<std::string, SDL_Texture*> textures;
    std::map<std::string, glm::vec2> textureSizes;
    // TODO: create map for fonts, audio, ...
    std::map<std::string, TTF_Font*> fonts;

public:
    AssetManager();
    ~AssetManager();

    void ClearAssets();
    void AddTexture(SDL_Renderer* renderer, const std::string& filepath);
    SDL_Texture* GetTexture(const std::string& filepath);
    glm::vec2 GetTextureSize(const std::string& filepath);

    void AddFont(const std::string& filepath, int fontSize);
    TTF_Font* GetFont(const std::string& filepath);
};

#endif // ASSETMANAGER_H