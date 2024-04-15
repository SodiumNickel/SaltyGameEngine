#ifndef EDITHISTORY_H
#define EDITHISTORY_H

#include "Edit.h"
#include "../EngineData.h"
#include <stack>
#include <memory>

class EditHistory {
    private:
        std::shared_ptr<EngineData> engineData;

        std::stack<Edit*> undoStack;
        std::stack<Edit*> redoStack;
    public:
        EditHistory(std::shared_ptr<EngineData> engineData) : engineData(engineData) {};

        bool unsaved = false;
        bool canUndo = false;
        bool canRedo = false;
        // Writes to current-scene.json and pushes to undoStack. Clears redoStack.
        void Do(Edit* action);
        // Modifies entityTree + current-scene.json and pushes to redoStack 
        void Undo();
        void Redo();
        void Save();
};

#endif