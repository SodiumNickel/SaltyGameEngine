#include "EditHistory.h"

#include "Edit.h"
#include <stack>

void EditHistory::Do(Edit* action){
    action->ApplyJson(false);
    unsaved = true;

    undoStack.push(action);
    // redoStack.clear(); TODO: REMEMBER TO DEALLOCATE POINTERS, or just use unique_ptrs (shared?)
}

// Pre: !undoStack.empty
void EditHistory::Undo(){ 
    undoStack.top()->Apply(true);
    redoStack.push(undoStack.top());
    undoStack.pop(); // TODO: might need to deallocate this
}

// Pre: !redoStack.empty
void EditHistory::Redo(){
    redoStack.top()->Apply(false);
    undoStack.push(redoStack.top());
    redoStack.pop();  // TODO: might need to deallocate this
}

// Pre: unsaved = true;
void EditHistory::Save(){
    // copy current-scene into actual scene
    unsaved = false;
}