#ifndef EDITHISTORY_H
#define EDITHISTORY_H

#include "Edit.h"
#include <stack>

class EditHistory {
    private:
        std::stack<Edit*> undoStack;
        std::stack<Edit*> redoStack;
        // I: |actionStack| = |undoStack|
        // Stores each action to be pushed into redo stack when an undo is called
        std::stack<Edit*> actionStack;
    public:
        // Writes to current-scene.json and pushes to undoStack. Clears redoStack.
        void Do(Edit* action, Edit* reverse);
        // Modifies entityTree + current-scene.json and pushes to redoStack 
        void Undo();
        void Redo();
};

#endif