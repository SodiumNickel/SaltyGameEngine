#include "EditHistory.h"

#include "Edit.h"
#include <stack>

void EditHistory::Do(Edit* action, Edit* reverse){
    action->ApplyJson();
    undoStack.push(reverse);
    actionStack.push(action);

    // redoStack.clear(); TODO: REMEMBER TO DEALLOCATE POINTERS, or just use unique_ptrs
}

// Pre: !undoStack.empty
void EditHistory::Undo(){ 
    undoStack.top()->Apply();
    undoStack.pop();
    redoStack.push(actionStack.top());
    actionStack.pop();
}

// Pre: !redoStack.empty
void EditHistory::Redo(){

}