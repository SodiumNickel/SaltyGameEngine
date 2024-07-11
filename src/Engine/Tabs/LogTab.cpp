#include "EnGUI.h"

#include <iostream>

#include <imgui.h>

#include "../Debug/SaltyDebug.h"


void LogTab::Begin(){
    ImGui::Begin("Debug Log");

    for(LogEntry entry : Debug::logEntries){
        ImGui::Text(entry.message.c_str());
    }

    ImGui::End();
}