// USER SCRIPT INCLUDES - written by engine

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
};

// USER SCRIPT CONSTRUCTORS - written by engine