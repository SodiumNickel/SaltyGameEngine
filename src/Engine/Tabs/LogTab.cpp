#include "Engine/Tabs/Tabs.h"

#include <iostream>

#include <imgui.h>

#include "Engine/Debug/SaltyDebug.h"


void LogTab::Begin(){
    ImGui::Begin("Debug Log");

    for(LogEntry entry : Debug::LogEntries){
        ImGui::Text(entry.message.c_str());
    }

    ImGui::End();
}