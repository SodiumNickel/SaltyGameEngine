#ifndef MENU_H
#define MENU_H

#include <vector>

#include "Engine/EngineData.h"
#include "Engine/History/Edit.h"
#include "Engine/History/EditHistory.h"
#include "Engine/Stage/Stage.h"

#include "Game/ECS/ECS.h"


class Menu {
private:
    std::shared_ptr<Registry> registry;
    std::shared_ptr<Stage> stage;
    std::shared_ptr<EngineData> engineData;
    std::shared_ptr<EditHistory> editHistory;

    /* Export Handling */
    void ExportPopup();
    void ExportWindows();
    void ExportWeb();

    /* FPS Display */
    bool showFps = false;
    void FPSMetric();
    // Average over last n frames (2 seconds)
    int n;
    std::vector<float> lastNFrames;
    float totalNFrames = 0; 
    int lastFrameIndex = 0;
    // Stores the number of frames in a row this fps has been read
    int sameFpsInRow = 0;
    int prevFps = 0; 
    // Stores the currently displayed fps
    int displayedFps = 0;
public:
    Menu(std::shared_ptr<Registry> registry, std::shared_ptr<Stage> stage, std::shared_ptr<EngineData> engineData, std::shared_ptr<EditHistory> editHistory) 
    : registry(registry), stage(stage), engineData(engineData), editHistory(editHistory) 
    { n = 2000 / engineData->targetFrameTime; lastNFrames = std::vector<float>(n, 0.0f); }; // Will display average fps over last 2 seconds
    // TODO: make sure this indentation is same as rest of code ^^ (above)
    void Begin();
};

#endif // MENU_H