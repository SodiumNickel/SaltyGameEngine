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

    int n;
    std::vector<float> lastNFrames;
    float totalNFrames = 0; 
    int lastFrameIndex = 0;
public:
    Menu(std::shared_ptr<EngineData> engineData, std::shared_ptr<EditHistory> editHistory) : engineData(engineData), editHistory(editHistory) 
    { n = 1000 / engineData->targetFrameTime; lastNFrames = std::vector<float>(n, 0.0f); }; // TODO: make sure this indentation is same as rest of code
    void Begin();
};

class Tab {
public:
    virtual void Begin() = 0;    
};

class EntityTab : public Tab {
private:
    int index;
    std::shared_ptr<EditHistory> editHistory;
    std::shared_ptr<Stage> stage;
public:
    EntityTab(std::shared_ptr<EditHistory> editHistory, std::shared_ptr<Stage> stage) : editHistory(editHistory), stage(stage) {};
    void Begin() override;
};

class ComponentTab : public Tab {
private:
    int index;
    ComponentValue prev;
    std::shared_ptr<EditHistory> editHistory;
    std::shared_ptr<Stage> stage;
    
    Entity entity;
    int entityId;

    // TODO: this should just access entity by id from above
    void Transform();
    void Sprite();
    void Rigidbody();
    void BoxCollider();

    bool addComponentOpen = false;
public:
    ComponentTab(std::shared_ptr<EditHistory> editHistory, std::shared_ptr<Stage> stage) : editHistory(editHistory), stage(stage), prev() {};
    void Begin() override;
};

class ScriptTab : public Tab {
private:
    int index;
    std::shared_ptr<Stage> stage;
public:
    ScriptTab(std::shared_ptr<Stage> stage) : stage(stage) {};
    void Begin() override;
};

class AssetTab : public Tab {
private:
    int index;
    std::shared_ptr<Stage> stage;
public:
    AssetTab(std::shared_ptr<Stage> stage) : stage(stage) {};
    void Begin() override;
};

#endif