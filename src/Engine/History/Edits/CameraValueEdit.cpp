#include "Engine/History/Edit.h"

#include <fstream>
#include <iostream>
#include <variant>

#include <glm.hpp>
#include <json.hpp>
using json = nlohmann::json;

#include "Game/Salty/SaltyCamera.h"


/* -----CAMERA VALUE EDIT------------------------------------------------- *
 *   When the user changes a camera value (i.e. position, aspect ratio)    *
 * ----------------------------------------------------------------------- */
void CameraValueEdit::Apply(bool undo){
    ComponentValue& val = undo ? prev : cur; 
    switch(camVar) {
        case CAM_POSITION_X: Camera::position.x = std::get<float>(val); break;
        case CAM_POSITION_Y: Camera::position.y = std::get<float>(val); break;
        case CAM_AR_X: Camera::aspectRatio.x = std::get<int>(val); break;
        case CAM_AR_Y: Camera::aspectRatio.x = std::get<int>(val); break;
        case CAM_SCALE: Camera::scale = std::get<float>(val); break;
        default:
            // TODO: should log a -1 level error here
            break;
    }

    ApplyJson(undo);
}

void CameraValueEdit::ApplyJson(bool undo){
    ComponentValue& val = undo ? prev : cur; 
    std::ifstream g("EngineData/current-scene.json");
    json jScene = json::parse(g);
    json jCamera = jScene["camera"];

    switch(camVar) {
        case CAM_POSITION_X: jCamera["position"][0] = std::get<float>(val); break;
        case CAM_POSITION_Y: jCamera["position"][1] = std::get<float>(val); break;
        case CAM_AR_X: jCamera["aspectRatio"][0] = std::get<int>(val); break;
        case CAM_AR_Y: jCamera["aspectRatio"][1] = std::get<int>(val); break;
        case CAM_SCALE: jCamera["scale"] = std::get<float>(val); break;
        default:
            // TODO: should log a -1 level error here
            break;
    }

    jScene["camera"] = jCamera;
    std::ofstream("EngineData/current-scene.json") << std::setw(2) << jScene;
    g.close();
}

// TODO: dont think this should ever be false? not sure though
bool CameraValueEdit::ValidEdit(){
    return true;
}

std::string CameraValueEdit::ToString(bool undo){
    return "Camera Value Edit";
}