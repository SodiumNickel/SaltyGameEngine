#ifndef EDIT_H
#define EDIT_H

#include <string>
#include <variant>

#include "Engine/EngineData.h"

#include "Game/ECS/ECS.h"
#include "Game/Salty/SaltyTypes.h"
#include "Game/Salty/SaltyDebug.h"

class IEdit {
public:
    virtual void Apply(bool undo) = 0;
    virtual void ApplyJson(bool undo) = 0;
    virtual bool ValidEdit() = 0;
    virtual std::string ToString(bool undo) = 0;
};

/* -----COMPONENT EDITS---------------------------------- *
 *   Editing values, adding/removing                  *
 * ------------------------------------------------------ */

enum EComponentTypes {
    NAME, // Okay, entity.name is not technically a component, but using ComponentValueEdit is convenient for name edits
    TRANSFORM,
    SPRITE,
    RIGIDBODY,
    BOXCOL
};
// TODO: check out inline constexpr with std::array<std::string_view>
enum EComponentVars {
    // TRANSFORM
    POSITION_X,
    POSITION_Y,
    SCALE_X,
    SCALE_Y,
    ROTATION,
    // SPRITE
    FILEPATH,
    ZINDEX,
    // RIGIDBODY
    INITVEL_X,
    INITVEL_Y
};

// Used to store changes made to components
typedef std::variant<int, float, std::string> ComponentValue;

// When the user edits a value in the component, i.e. transform.position.x from 0.0f to 1.0f
class ComponentValueEdit : public IEdit {
private:    
    EComponentTypes compType;
    EComponentVars compVar;
    std::shared_ptr<Registry> registry;
    int entityId;
    ComponentValue prev; // Used to undo action
    ComponentValue cur; // Used to (re)do action
public:
    ComponentValueEdit(EComponentTypes compType, EComponentVars compVar, std::shared_ptr<Registry> registry, int entityId, ComponentValue prev, ComponentValue cur): 
        compType(compType), compVar(compVar), registry(registry), entityId(entityId), prev(prev), cur(cur) {};
    void Apply(bool undo) override;
    void ApplyJson(bool undo) override;
    bool ValidEdit() override;
    std::string ToString(bool undo) override;
};

// When the user adds or removes a component
class HasComponentEdit : public IEdit {
private:
    EComponentTypes compType;
    std::shared_ptr<Registry> registry;
    int entityId;
    // Contains all values in deleted/added component (or is empty if they are all default)
    std::vector<ComponentValue> values;
    // If the initial action was AddComponent (e.g. add = true -> undo() = RemoveComponent)
    bool add;
public:
    HasComponentEdit(EComponentTypes compType, std::shared_ptr<Registry> registry, int entityId, bool add, std::vector<ComponentValue> values): 
        compType(compType), registry(registry), entityId(entityId), add(add), values(values) {}; // TODO: not sure if this is copying entire vector, we should not do that
    void Apply(bool undo) override;
    void ApplyJson(bool undo) override;
    bool ValidEdit() override;
    std::string ToString(bool undo) override;
};

/* -----CAMERA MANAGEMENT-------------------------------- *
 *   Changing position, aspect ratio, scale               *
 * ------------------------------------------------------ */
enum ECameraVars {
    CAM_POSITION_X, 
    CAM_POSITION_Y,
    CAM_AR_X,
    CAM_AR_Y,
    CAM_SCALE
};

class CameraValueEdit : public IEdit {
private:
    ECameraVars camVar;
    ComponentValue prev;
    ComponentValue cur;
public:
    CameraValueEdit(ECameraVars camVar, ComponentValue prev, ComponentValue cur)
    : camVar(camVar), prev(prev), cur(cur) {};
    void Apply(bool undo) override;
    void ApplyJson(bool undo) override;
    bool ValidEdit() override;
    std::string ToString(bool undo) override;
};

/* -----SCRIPT MANAGEMENT-------------------------------- *
 *   Adding/removing scripts, editing SF_ values          *
 * ------------------------------------------------------ */

// When the user edits a value one of their scripts
class ScriptValueEdit : public IEdit {
private:    
    std::shared_ptr<EngineData> engineData;
    int entityId;
    std::string filepath;
    std::string type;
    std::string name;
    SaltyType prev; // Used to undo action
    SaltyType cur; // Used to (re)do action // TODO: should prolly be saltytype ref
public:
    ScriptValueEdit(std::shared_ptr<EngineData> engineData, int entityId, std::string filepath, std::string type, std::string name, SaltyType prev, SaltyType cur): 
        engineData(engineData), entityId(entityId), filepath(filepath), type(type), name(name), prev(prev), cur(cur) {};
    void Apply(bool undo) override;
    void ApplyJson(bool undo) override;
    bool ValidEdit() override;
    std::string ToString(bool undo) override;
};

// When the user adds or removes a script from an entity
class HasScriptEdit : public IEdit {
private:
    std::shared_ptr<EngineData> engineData;
    int entityId;
    ScriptData scriptData;
    bool add;
public:
    HasScriptEdit(std::shared_ptr<EngineData> engineData, int entityId, ScriptData scriptData, bool add):
        engineData(engineData), entityId(entityId), scriptData(scriptData), add(add) {};
    void Apply(bool undo) override;
    void ApplyJson(bool undo) override;
    bool ValidEdit() override;
    std::string ToString(bool undo) override;
};

// NOTE: DELETING AN ENTITY HAS TO PUT IT IN SAME PLACE AFTER UNDO, OTHERWISE THIS WHOLE SYSTEM BREAKS

/* -----ENTITY MANAGEMENT-------------------------------- *
 *   Reparenting/moving, adding/removing                  *
 * ------------------------------------------------------ */

// When the user reparents or moves an entity
class ReparentEdit : public IEdit {
private:
    std::shared_ptr<Registry> registry;

    int entityId;
    int prevParentId;
    int prevPos;
    int curParentId;
    int curPos;
public:
    ReparentEdit(std::shared_ptr<Registry> registry, int entityId, int prevParentId, int prevPos, int curParentId, int curPos)
    : registry(registry), entityId(entityId), prevParentId(prevParentId), prevPos(prevPos), curParentId(curParentId), curPos(curPos) {};
    void Apply(bool undo) override;
    void ApplyJson(bool undo) override;
    bool ValidEdit() override;
    std::string ToString(bool undo) override;
};

class EntityExistsEdit : public IEdit {
private:
    std::shared_ptr<Registry> registry;
    std::shared_ptr<EngineData> engineData;

    int entityId;
    std::string name;
    int parentId;
    int pos;
    bool root; // If the entity was at the root of the tree deleted

    bool add;
    // Needs to store vector of components, to restore them after
    std::vector<float> transformValues;
    std::vector<std::unique_ptr<HasComponentEdit>> components;
    // Needs to store vector of scripts to restore them after
    std::vector<std::unique_ptr<HasScriptEdit>> scripts;

    // Storing children-ids for json edits
    std::vector<int> childrenIds;

    // Needs to store edits to re-add children
    std::vector<std::unique_ptr<EntityExistsEdit>> childrenEdits;
public:
    // Defined in EntityExistsEdit.cpp
    EntityExistsEdit(std::shared_ptr<Registry> registry, std::shared_ptr<EngineData> engineData, int entityId, std::string name, int parentId, int pos, bool root, bool add);
    void Apply(bool undo) override;
    void ApplyJson(bool undo) override;
    bool ValidEdit() override;
    std::string ToString(bool undo) override;
};

#endif // EDIT_H