#ifndef ENGUI_H
#define ENGUI_H

#include "./EngineData.h"
#include "./History/Edit.h"
#include "./History/EditHistory.h"
#include "./Stage/Stage.h"

class Menu {
private:
    std::shared_ptr<EngineData> engineData; // TODO: engine data is unused rn, not sure if itll be useful later
    std::shared_ptr<EditHistory> editHistory;
public:
    Menu(std::shared_ptr<EngineData> engineData, std::shared_ptr<EditHistory> editHistory) : engineData(engineData), editHistory(editHistory) {};
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