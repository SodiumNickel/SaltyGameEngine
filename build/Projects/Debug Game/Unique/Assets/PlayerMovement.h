#pragma once
#include "SaltyEngine.h"

// TODO: eventually might make a text editor abstraction for this
class PlayerMovement : public IScript {
// TODO: remove comments from section, also find a better way of finding the location of rivate and ublic.
private: 
    float speed = 80.0f;
    string name;
    SF_ int val; // TODO: might do opposite actually, include a def for ones that should be serialized, maybe just SF?
    SF_ float timer;

    // Non serialized var
    SF_ string tag;

    // Sprite* test;
    SF_ Transform* player;
    
    Sound sound;
public:
    // Initialization will be handled by engine (including that of SF_ variables)
    PlayerMovement(Entity* entity, Transform* transform, std::vector<SaltyType>& serializedVars);

    void Start() override;
    void Update(float dt) override;
};