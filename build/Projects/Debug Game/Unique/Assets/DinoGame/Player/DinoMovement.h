#pragma once
#include "SaltyEngine.h"

class DinoMovement : public IScript {
private:
    // Sprites (dr=DinoRun)
    SF_ Sprite* sprite;
    SF_ string dr1;
    SF_ string dr2;
    SF_ string dr3;
    SF_ string dr4;
    SF_ string dr5;
    SF_ string dr6;

    // Animation states
    int runState = 1;
    SF_ float runFrameTime;
    float runTimer;
public:
    // Initialization will be handled by engine (including that of SF_ variables)
    DinoMovement(Entity* entity, Transform* transform, std::vector<SaltyType>& serializedVars);

    void Start() override;
    void Update(float dt) override;
};