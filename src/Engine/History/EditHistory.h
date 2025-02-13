#ifndef EDITHISTORY_H
#define EDITHISTORY_H

#include <memory>
#include <stack>

#include "Engine/EngineData.h"
#include "Engine/History/Edit.h"


class EditHistory {
    private:
        std::shared_ptr<EngineData> engineData;

        std::stack<std::unique_ptr<IEdit>> undoStack;
        std::stack<std::unique_ptr<IEdit>> redoStack;
    public:
        EditHistory(std::shared_ptr<EngineData> engineData) : engineData(engineData) {};

        bool unsaved = false;
        bool canUndo = false;
        bool canRedo = false;
        // Writes to current-scene.json and pushes to undoStack. Clears redoStack.
        void Do(std::unique_ptr<IEdit> action);
        // Modifies entityTree + current-scene.json and pushes to redoStack 
        void Undo();
        void Redo();
        void Save();
        // Clears undoStack, redoStack, and save state (currently used when switching scenes)
        void Clear();
};

#endif // EDITHISTORY_H