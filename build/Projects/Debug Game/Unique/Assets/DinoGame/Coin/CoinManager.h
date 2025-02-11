#pragma once
#include "SaltyEngine.h"

class CoinManager : public IScript {
private:
    // Coin transforms
    SF_ Transform* coin1;
    SF_ Transform* coin2;
    SF_ Transform* coin3;
    SF_ Transform* coin4;
    SF_ Transform* coin5;
    SF_ Transform* coin6;

    // Coin rigidbodies
    SF_ Rigidbody* rb1;
    SF_ Rigidbody* rb2;
    SF_ Rigidbody* rb3;
    SF_ Rigidbody* rb4;
    SF_ Rigidbody* rb5;
    SF_ Rigidbody* rb6;

    // To compare coins to players
    SF_ Transform* playerTransform;

    // Coin collect sounds
    SF_ Sound collect1;
    SF_ Sound collect2;
    SF_ Sound collect3;
    SF_ Sound collect4;
    SF_ Sound collect5;
    int soundIdx;

    // BG Music
    SF_ Sound music;
    float musicTimer;
public:
    // Initialization will be handled by engine (including that of SF_ variables)
    CoinManager(Entity* entity, Transform* transform, std::vector<SaltyType>& serializedVars);

    void Start() override;
    void Update(float dt) override;
};