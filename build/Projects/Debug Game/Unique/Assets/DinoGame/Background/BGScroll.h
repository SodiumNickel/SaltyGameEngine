#pragma once
#include "SaltyEngine.h"

class BGScroll : public IScript {
private:
    SF_ float scrollSpeed;
    float maxDt;
public:
    // Initialization will be handled by engine (including that of SF_ variables)
    BGScroll(Entity* entity, Transform* transform, std::vector<SaltyType>& serializedVars);

    void Start() override;
    void Update(float dt) override;
};