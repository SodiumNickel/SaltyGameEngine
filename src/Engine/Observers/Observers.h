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
    virtual bool Check() = 0;
    virtual void Observe() = 0;    
};

// When the user edits a custom script in external code editor of choice
class ScriptObserver : public IObserver {
private:
    std::shared_ptr<EditHistory> editHistory;
    std::shared_ptr<EngineData> engineData;

public:
    ScriptObserver(std::shared_ptr<EngineData> engineData, std::shared_ptr<EditHistory> editHistory) 
    : engineData(engineData), editHistory(editHistory) {};
    bool Check() override;
    void Observe() override;
};

#endif // OBSERVERS_H