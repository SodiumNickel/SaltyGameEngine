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

    // Jump vars
    bool canJump = true;
    bool jumping = false;
    float fallTimer = 0;
    SF_ float jumpTime;
    SF_ float jumpSpeed;
    SF_ float fallSpeed;
    float jumpTimer;
    SF_ float minY;
    // Allows player to press jump before they hit the ground
    float prejumpTimer;
    // Sets minimum jump height
    float justJumpedTimer;

    // Jump vars
    SF_ Sound jump1;
    SF_ Sound jump2;
    SF_ Sound jump3;
    SF_ Sound jump4;
    SF_ Sound jump5;
    int soundIdx = 0;
public:
    // Initialization will be handled by engine (including that of SF_ variables)
    DinoMovement(Entity* entity, Transform* transform, std::vector<SaltyType>& serializedVars);

    void Start() override;
    void Update(float dt) override;
};