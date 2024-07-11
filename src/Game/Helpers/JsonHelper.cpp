#include "JsonHelper.h"


glm::vec2 JsonToVec2(json array){
    float x = array[0];
    float y = array[1];
    return glm::vec2(x, y);
}
