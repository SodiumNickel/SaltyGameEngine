#include "EditHistory.h"

#include "Edit.h"
#include <stack>

void EditHistory::Undo(){
    undoStack.push(new ComponentValueEdit());
    undoStack.top()->Apply();
    undoStack.pop();
}