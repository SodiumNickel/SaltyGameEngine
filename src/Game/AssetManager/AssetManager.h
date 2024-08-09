#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H

#include <map>
#include <memory>
#include <string>

#include <SDL.h>
#include <SDL_ttf.h>
#include <glm.hpp>
#include <soloud.h>

class AssetManager {
private:
    std::map<std::string, SDL_Texture*> textures;
    std::map<std::string, glm::vec2> textureSizes;
    // TODO: might have seperate font system later (with textures already made)
    std::map<std::string, TTF_Font*> fonts;
    // SoLoud::AudioSource is superclass of Wav and WavStream
    std::map<std::string, std::unique_ptr<SoLoud::AudioSource>> sounds;
public:
    AssetManager();
    ~AssetManager();

    void ClearAssets();

    void AddTexture(SDL_Renderer* renderer, const std::string& filepath);
    SDL_Texture* GetTexture(const std::string& filepath);
    glm::vec2 GetTextureSize(const std::string& filepath);

    void AddFont(const std::string& filepath, int fontSize);
    TTF_Font* GetFont(const std::string& filepath);

    void AddSound(const std::string& filepath, bool streamed);
};

#endif // ASSETMANAGER_H