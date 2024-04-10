#ifndef ENGUI_H
#define ENGUI_H

#include "./Stage/Stage.h"

void Menu();

class Tab {
public:
    virtual void Begin() = 0;    
};

class EntityTab : public Tab {
private:
    int index;
    Stage& stage;
public:
    EntityTab(Stage& stage) : stage(stage) {};
    void Begin() override;
};

class ComponentTab : public Tab {
private:
    int index;
    Stage& stage;
public:
    ComponentTab(Stage& stage) : stage(stage) {};
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