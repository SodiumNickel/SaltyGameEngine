// TODO: also want to include the SaltyEngine.h (including debug, input, etc.)
#include "Game/Salty/SaltyInput.h"
#include "Game/ECS/ECS.h" // TODO: put into salty engine, also put all components

#include "Game/Salty/SaltyAudio.h"

#include <SDL.h>

// TODO: think about
// TODO: dont think LOCAL is the best name for this, but will use a similar method to keep vars not serialized
// Used to keep out of script tab in engine, and wont be initialized
#define LOCAL_ // TODO: want some differentiating thing, so it cant be a type, i dont like the _ but maybe somethign?
#define SF_ // Think i will stick with the _ after the tag
#define SF
#define _SF
#define SLIDER(min, max) // Except for ones with arguments? maybe just do this one in engine?
// Should also have ones for like headers and stuff for making engine organization easier

#include <variant>
#include <vector>

#include "Game/Salty/SaltyTypes.h"

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
    PlayerMovement(Entity* entity, Transform* transform);
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