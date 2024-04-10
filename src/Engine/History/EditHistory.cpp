#include "EditHistory.h"

#include "Edit.h"
#include <stack>

void EditHistory::Do(){
    //undoStack.push(new ComponentValueEdit());
}

void EditHistory::Undo(){
    undoStack.top()->Apply();
    undoStack.pop();
}

void EditHistory::Redo(){

}