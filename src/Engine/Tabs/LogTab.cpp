#include "Engine/Tabs/Tabs.h"

#include <iostream>

#include <imgui.h>

#include "Game/Salty/SaltyDebug.h"


void LogTab::Begin(){
    ImGui::Begin("Debug Log");

    for(LogEntry entry : Debug::LogEntries){
        ImGui::Text(entry.message.c_str());
    }

    ImGui::End();
}