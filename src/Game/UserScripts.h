#ifndef USERSCRIPTS_H
#define USERSCRIPTS_H
// TODO: this and json helper (i.e. as function local to file) should probably be removed/moved somewhere else
// but that is a later issue lol

// USER SCRIPT INCLUDES - written by engine

#include "Game/ECS/ECS.h"
#include "Game/Salty/SaltyTypes.h"

#include <map>
#include <memory>
#include <string>

template<typename TScript> 
IScript* createInstance(Entity* entity, Transform* transform, std::vector<SaltyType>& serializedVars) { 
    return new TScript(entity, transform, serializedVars); 
}

std::map<std::string, IScript*(*)(Entity*, Transform*, std::vector<SaltyType>&)> scriptMap = {
    // USER SCRIPT MAPPING - written by engine
};

#endif // USERSCRIPTS_H