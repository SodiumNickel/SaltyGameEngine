#pragma once
#include "SaltyEngine.h"

class ExportTests : public IScript {
private:
    SF_ int val;
    SF_ float timer;

    // Non serialized var
    SF_ string tag;

    SF_ Transform* player;
public:
    // Initialization will be handled by engine (including that of SF_ variables)
    ExportTests(Entity* entity, Transform* transform, std::vector<SaltyType>& serializedVars);

    void Start() override;
    void Update(float dt) override;
};