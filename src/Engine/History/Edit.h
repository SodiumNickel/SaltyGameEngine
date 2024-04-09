#ifndef EDIT_H

class Edit {
public:
    void Apply();
};

// still needs some initializer
class ComponentValueEdit : public Edit {
public:
    void Apply();
};

#endif