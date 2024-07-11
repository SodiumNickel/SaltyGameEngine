#include "Game/Helpers/JsonHelper.h"


glm::vec2 JsonToVec2(json jArray){
    float x = jArray[0];
    float y = jArray[1];
    return glm::vec2(x, y);
}
