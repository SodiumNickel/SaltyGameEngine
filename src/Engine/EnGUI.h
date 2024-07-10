#ifndef ENGUI_H
#define ENGUI_H

#include "./EngineData.h"
#include "./History/Edit.h"
#include "./History/EditHistory.h"
#include "./Stage/Stage.h"

#include <vector>

class Menu {
private:
    std::shared_ptr<EngineData> engineData; // TODO: engine data is unused rn, not sure if itll be useful later
    std::shared_ptr<EditHistory> editHistory;

    /* FPS Display */
    bool showFps = false;
    // Average over last n frames (2 seconds)
    int n;
    std::vector<float> lastNFrames;
    float totalNFrames = 0; 
    int lastFrameIndex = 0;
    // Stores the number of frames in a row this fps has been read
    int sameFpsInRow = 0;
    int prevFps = 0; 
    // Stores the currently displayed fps
    int displayedFps = 0;
public:
    Menu(std::shared_ptr<EngineData> engineData, std::shared_ptr<EditHistory> editHistory) : engineData(engineData), editHistory(editHistory) 
    { n = 2000 / engineData->targetFrameTime; lastNFrames = std::vector<float>(n, 0.0f); }; // Will display average fps over last 2 seconds
    // TODO: make sure this indentation is same as rest of code ^^ (above)
    void Begin();
};

// TODO: should be ITab
class Tab {
public:
    virtual void Begin() = 0;    
};

class EntityTab : public Tab {
private:
    int index;

    // Can be picked up by DD
    void DDSource(int id);
    // Can have DDSource placed into it
    void DDTarget(int id);
    // Right click to add child, remove, copy/paste, duplicate entity
    void RClickMenu(int id);

    std::shared_ptr<EditHistory> editHistory;
    std::shared_ptr<EngineData> engineData;
    std::shared_ptr<Registry> registry;
public:
    EntityTab(std::shared_ptr<EngineData> engineData, std::shared_ptr<EditHistory> editHistory, std::shared_ptr<Registry> registry) 
    : engineData(engineData), editHistory(editHistory), registry(registry) {};
    void Begin() override;
};

class ComponentTab : public Tab {
private:
    int index;
    ComponentValue prev;
    std::shared_ptr<EditHistory> editHistory;
    std::shared_ptr<EngineData> engineData; // TODO: not sure i will keep this in the end, currently just for selectedEntity
    std::shared_ptr<Registry> registry;
    std::shared_ptr<AssetManager> assetManager;

    Entity entity;
    int entityId;

    int selectedEntity;

    // TODO: this should just access entity by id from above
    void Transform();
    void Sprite();
    void Rigidbody();
    void BoxCollider();
    // Used to show close button on CollapsingHeader
    bool notRemoved = true; 

    bool addComponentOpen = false;
public:
    ComponentTab(std::shared_ptr<EngineData> engineData, std::shared_ptr<EditHistory> editHistory, std::shared_ptr<Registry> registry, std::shared_ptr<AssetManager> assetManager) 
    : engineData(engineData), editHistory(editHistory), registry(registry), assetManager(assetManager), prev() {};
    void Begin() override;
};

class ScriptTab : public Tab {
private:
    int index;
    std::shared_ptr<Registry> registry;
public:
    ScriptTab(std::shared_ptr<Registry> registry) : registry(registry) {};
    void Begin() override;
};

class AssetTab : public Tab {
private:
    int index;
    std::shared_ptr<Registry> registry;

    std::filesystem::path currentDir = std::filesystem::path("./Unique/Assets");
public:
    AssetTab(std::shared_ptr<Registry> registry) : registry(registry) {};
    void Begin() override;
};

class LogTab : public Tab {
private:
    int index;
    std::shared_ptr<Registry> registry; // TODO: doesnt need registry, but might need other inits so just gonna leave the formatting here for now lol
public:
    LogTab(std::shared_ptr<Registry> registry) : registry(registry) {};
    void Begin() override;
};

#endif