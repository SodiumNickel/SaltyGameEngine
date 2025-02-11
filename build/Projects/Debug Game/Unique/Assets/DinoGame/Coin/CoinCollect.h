#pragma once
#include "SaltyEngine.h"

class CoinCollect : public IScript {
private:
    // Sprite filepaths for animation
    SF_ string coin1;
    SF_ string coin2;
    SF_ string coin3;
    SF_ string coin4;
    SF_ string coin5;
    SF_ string coin6;
    SF_ Sprite* sprite;
    float spinTimer;
    int spinState;
public:
    // Initialization will be handled by engine (including that of SF_ variables)
    CoinCollect(Entity* entity, Transform* transform, std::vector<SaltyType>& serializedVars);

    void Start() override;
    void Update(float dt) override;
};