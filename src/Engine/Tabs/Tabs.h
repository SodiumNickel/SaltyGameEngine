#ifndef TABS_H
#define TABS_H

#include <filesystem>
#include <vector>

#include "Engine/EngineData.h"
#include "Engine/History/Edit.h"
#include "Engine/History/EditHistory.h"

#include "Game/AssetManager/AssetManager.h"

class ITab {
public:
    virtual void Begin() = 0;    
};

class EntityTab : public ITab {
private:
    int index; // TODO: still dont know why i keep this lol, probably to delete from vector later??

    // Can be picked up by DD
    void DDSource(int id);
    // Can have DDSource placed into it
    void DDTarget(int id);
    // Right click to add child, remove, copy/paste, duplicate entity
    void RClickMenu(int id);

    std::shared_ptr<EditHistory> editHistory;
    std::shared_ptr<EngineData> engineData;
    std::shared_ptr<Registry> registry;

    // Used to open entity when a child is dropped into it
    // NOTE: only one entity should need to be forced open at any time. This is a non-issue as to drop child into parent,
    // all grandparents must already be open (otherwise we parent cannot be accessed)
    int forceOpen = -1;
public:
    EntityTab(std::shared_ptr<EngineData> engineData, std::shared_ptr<EditHistory> editHistory, std::shared_ptr<Registry> registry) 
    : engineData(engineData), editHistory(editHistory), registry(registry) {};
    void Begin() override;
};

class ComponentTab : public ITab {
private:
    int index;

    // Used to store previous values for Undo
    int previ;
    float prevf;
    std::string prevs;

    std::shared_ptr<EditHistory> editHistory;
    std::shared_ptr<EngineData> engineData;
    std::shared_ptr<Registry> registry;
    std::shared_ptr<AssetManager> assetManager;

    // Lock prevents selected entity from updating (with some edge cases surrounding -1)
    bool locked = false;
    int selectedEntity;

    void Transform();
    void Sprite();
    void Rigidbody();
    void BoxCollider();

    // Used to show close button on CollapsingHeader
    bool notRemoved = true; 

    bool addComponentOpen = false;
public:
    ComponentTab(std::shared_ptr<EngineData> engineData, std::shared_ptr<EditHistory> editHistory, std::shared_ptr<Registry> registry, std::shared_ptr<AssetManager> assetManager) 
    : engineData(engineData), editHistory(editHistory), registry(registry), assetManager(assetManager) {};
    void Begin() override;
};

class ScriptTab : public ITab {
private:
    int index;

    std::string prevs;

    std::shared_ptr<EditHistory> editHistory;
    std::shared_ptr<EngineData> engineData;
    std::shared_ptr<Registry> registry;
    
    bool locked = false;
    int selectedEntity;
public:
    ScriptTab(std::shared_ptr<EngineData> engineData, std::shared_ptr<EditHistory> editHistory, std::shared_ptr<Registry> registry) 
    : editHistory(editHistory), engineData(engineData), registry(registry) {}; // TODO: really need to unify order here
    void Begin() override;
};

class AssetTab : public ITab {
private:
    int index;
    std::shared_ptr<Registry> registry;

    std::filesystem::path currentDir = std::filesystem::path("./Unique/Assets");
public:
    AssetTab(std::shared_ptr<Registry> registry) : registry(registry) {};
    void Begin() override;
};

class LogTab : public ITab {
private:
    int index;
    std::shared_ptr<Registry> registry; // TODO: doesnt need registry, but might need other inits so just gonna leave the formatting here for now lol
public:
    LogTab(std::shared_ptr<Registry> registry) : registry(registry) {};
    void Begin() override;
};

#endif // TABS_H