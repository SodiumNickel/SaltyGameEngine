#ifndef ENGUI_H
#define ENGUI_H

#include "./History/Edit.h"
#include "./History/EditHistory.h"
#include "./Stage/Stage.h"

class Menu {
private:
    EditHistory& editHistory; // TODO: all of these can probably be pointers, dont need to be a reference
public:
    Menu(EditHistory& editHistory) : editHistory(editHistory) {};
    void Begin();
};

class Tab {
public:
    virtual void Begin() = 0;    
};

class EntityTab : public Tab {
private:
    int index;
    EditHistory& editHistory;
    Stage& stage;
public:
    EntityTab(EditHistory& editHistory, Stage& stage) : editHistory(editHistory), stage(stage) {};
    void Begin() override;
};

class ComponentTab : public Tab {
private:
    int index;
    ComponentValue prev;
    EditHistory& editHistory;
    Stage& stage;
    //int entityId;

    // TODO: this should just access entity by id from above
    void Transform(Entity entity);
public:
    ComponentTab(EditHistory& editHistory, Stage& stage) : editHistory(editHistory), stage(stage), prev() {};
    void Begin() override;
};

class ScriptTab : public Tab {
private:
    int index;
    Stage& stage;
public:
    ScriptTab(Stage& stage) : stage(stage) {};
    void Begin() override;
};

class AssetTab : public Tab {
private:
    int index;
    Stage& stage;
public:
    AssetTab(Stage& stage) : stage(stage) {};
    void Begin() override;
};

#endif