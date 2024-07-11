#ifndef JSONHELPER_H
#define JSONHELPER_H

#include <glm.hpp>
#include <json.hpp>
using json = nlohmann::json;


// returns glm::vec2 of floats
glm::vec2 JsonToVec2(json jArray);

#endif