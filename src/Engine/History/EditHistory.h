#ifndef EDITHISTORY_H
#define EDITHISTORY_H

#include <memory>
#include <stack>

#include "Engine/EngineData.h"
#include "Engine/History/Edit.h"


class EditHistory {
    private:
        std::shared_ptr<EngineData> engineData;

        std::stack<IEdit*> undoStack;
        std::stack<IEdit*> redoStack;
    public:
        EditHistory(std::shared_ptr<EngineData> engineData) : engineData(engineData) {};

        bool unsaved = false;
        bool canUndo = false;
        bool canRedo = false;
        // Writes to current-scene.json and pushes to undoStack. Clears redoStack.
        void Do(IEdit* action);
        // Modifies entityTree + current-scene.json and pushes to redoStack 
        void Undo();
        void Redo();
        void Save();
};

#endif // EDITHISTORY_H