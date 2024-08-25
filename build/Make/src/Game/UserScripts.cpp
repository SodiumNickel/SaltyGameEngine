// USER SCRIPT INCLUDES - written by engine
#include "PlayerMovement.h"

#include "Game/ECS/ECS.h"
#include "Game/Salty/SaltyTypes.h"

#include <map>
#include <memory>
#include <string>

template<typename TScript> 
IScript* CreateInstance(Entity* entity, Transform* transform, std::vector<SaltyType>& serializedVars) { 
    return new TScript(entity, transform, serializedVars); 
}

std::map<std::string, IScript*(*)(Entity*, Transform*, std::vector<SaltyType>&)> scriptMap = {
// USER SCRIPT MAPPING - written by engine
{"PlayerMovement", &CreateInstance<PlayerMovement>}
};

// USER SCRIPT CONSTRUCTORS - written by engine
PlayerMovement::PlayerMovement(Entity* entity, Transform* transform, std::vector<SaltyType>& serializedVars)
: IScript(entity, transform), 
speed(std::get<float>(serializedVars[0])), rb(std::get<Rigidbody*>(serializedVars[1])), jumpSound(std::get<Sound>(serializedVars[2])){};

