#ifndef EDIT_H

class Edit {
public:
    virtual void Apply() = 0;
};

// still needs some initializer
class ComponentValueEdit : public Edit {
    int entityId;
    // maybe a reference to the actual value? e.g. &transform.position.x
    // would prefer it to be the component itself and i could change it from the entity
public:
    ComponentValueEdit(int entityId): entityId(entityId) {};
    void Apply() override;
};

// NOTE: DELETING AN ENTITY HAS TO PUT IT IN SAME PLACE AFTER UNDO, OTHERWISE THIS WHOLE SYSTEM BREAKS

#endif