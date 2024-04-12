#ifndef EDITHISTORY_H
#define EDITHISTORY_H

#include "Edit.h"
#include <stack>

class EditHistory {
    private:
        std::stack<Edit*> undoStack;
        std::stack<Edit*> redoStack;
    public:
        // Writes to current-scene.json and pushes to undoStack. Clears redoStack.
        void Do(Edit* action);
        // Modifies entityTree + current-scene.json and pushes to redoStack 
        void Undo();
        void Redo();
};

#endif