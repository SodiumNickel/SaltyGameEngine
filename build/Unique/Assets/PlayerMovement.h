#include "SaltyEngine.h"

// TODO: eventually might make a text editor abstraction for this
class PlayerMovement : public IScript {
// TODO: remove comments from section, also find a better way of finding the location of rivate and ublic.
private: 
    float speed = 30.0f;
    string name;
    SF_ int val; // TODO: might do opposite actually, include a def for ones that should be serialized, maybe just SF?

    // Sprite* test;
    SF_ Transform* player;

    Sound sound;
public:
    // Initialization will be handled by engine (including that of SF_ variables)
    // PlayerMovement(Entity* entity, Transform* transform) {};
    // TODO: just add a new constructor with everything

    // TODO: actually just constructor which takes those two things, and then an array of std::variant<SaltyTypes>
    // which just takes valid variable types and then assigns them (written in build)
    PlayerMovement(Entity* entity, Transform* transform, std::vector<SaltyType>& serializedVars) // TODO: didnt want this to be a vector, but not sure i can get arroundd it here?
    : IScript(entity, transform), 
    val(std::get<int>(serializedVars[0])), player(std::get<Transform*>(serializedVars[1])) 
    {};

    void Start() override;
    void Update(float dt) override;
};