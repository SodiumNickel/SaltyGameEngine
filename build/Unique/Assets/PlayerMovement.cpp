#include "Game/Scripts/Script.h" // TODO: I want this to be a better path
// TODO: also want to include the SaltyEngine.h (including debug, input, etc.)
#include "Game/Input/SaltyInput.h"
#include "Game/ECS/ECS.h" // TODO: put into salty engine, also put all components
#include "Game/Components/TransformComponent.h"
#include "Game/Components/SpriteComponent.h"

#include <SDL.h>

// TODO: think about
// TODO: dont think LOCAL is the best name for this, but will use a similar method to keep vars not serialized
// Used to keep out of script tab in engine, and wont be initialized
#define LOCAL
// Should also have ones for like headers and stuff for making engine organization easier
typedef std::string string; 
// I think this is good to have for convenience here
typedef TransformComponent* Transform;
typedef SpriteComponent* Sprite;
// TODO: with above, when you do GetComponent<> is has to be the full SpriteComponent...
// semi okay with this tbh

class PlayerMovement : public IScript {
// FOR NOW: ASSUMING THERE ARE NO COMMENTS BETWEEN PRIVATE AND PUBLIC
// TODO: remove comments from section, also find a better way of finding the location of rivate and ublic.
private: 
    LOCAL float speed = 30.0f;
    string name;
    int val;
    
    Sprite test;
public:
    // Initialization will be handled by engine
    PlayerMovement(Entity* entity, Transform transform);

    void Start() override;
    void Update(float deltaTime) override;
};

// Called before the first frame of Update()
void PlayerMovement::Start(){

}

// TODO: may add more detail
// Called every frame before Render() 
void PlayerMovement::Update(float deltaTime){
    if(Input::KeyHeld[SDL_SCANCODE_A]){
        transform->position.x -= deltaTime * speed;
    }
    if(Input::KeyHeld[SDL_SCANCODE_D]){
        transform->position.x += deltaTime * speed;
    }

}