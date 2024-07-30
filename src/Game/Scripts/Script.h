#ifndef SCRIPT_H
#define SCRIPT_H
class IScript {
private:
    // Do i store owner entity and transform here?
public:
    virtual void Start() = 0; 
    virtual void Update() = 0;     
};

// TODO: this will be altered by engine to initialize script objects

#endif