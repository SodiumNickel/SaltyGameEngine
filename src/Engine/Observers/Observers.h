#ifndef OBSERVERS_H
#define OBSERVERS_H

#include <filesystem>
#include <vector>

#include "Engine/EngineData.h"
#include "Engine/History/Edit.h"
#include "Engine/History/EditHistory.h"

#include "Game/Salty/SaltyTypes.h"

class IObserver {
public:
    virtual void Observe() = 0;    
};

// When the user edits a custom script in external code editor of choice
class ScriptObserver : public IObserver {
private:
    std::shared_ptr<EditHistory> editHistory;
    std::shared_ptr<EngineData> engineData;

    SaltyType DefaultArg(std::string type);
    json DefaultArgJson(std::string type);
public:
    ScriptObserver(std::shared_ptr<EngineData> engineData, std::shared_ptr<EditHistory> editHistory) 
    : engineData(engineData), editHistory(editHistory) {};

    void Observe() override;
};

#endif // OBSERVERS_H