#include "EnGUI.h"
#include "../Debug/SaltyDebug.h"

#include <imgui.h>

#include <iostream>

void LogTab::Begin(){
    ImGui::Begin("Debug Log");

    for(LogEntry entry : Debug::logEntries){
        ImGui::Text(entry.message.c_str());
    }

    ImGui::End();
}