#ifndef EDITHISTORY_H

#include "Edit.h"
#include <stack>

class EditHistory {
    private:
        std::stack<Edit*> undoStack;
        std::stack<Edit*> redoStack;
    public:
        void Do();
        void Undo();
        void Redo();
};

#endif