#pragma once
#include "SaltyEngine.h"

class GroundScroll : public IScript {
private:
    float scrollSpeed = 15;
public:
    // Initialization will be handled by engine (including that of SF_ variables)
    GroundScroll(Entity* entity, Transform* transform, std::vector<SaltyType>& serializedVars);

    void Start() override;
    void Update(float dt) override;
};