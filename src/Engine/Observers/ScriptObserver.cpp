#include "Engine/Observers/Observers.h"

#include <fstream>
#include <string>
#include <vector>

#include <imgui.h>

#include "Game/Salty/SaltyDebug.h"

void ScriptObserver::Observe(){
    for (int i = 0; i < engineData->scriptFilepaths.size(); i++){
        // Full path from projects to header file
        std::string filepath = engineData->assetsRootDir + '/' + engineData->scriptFilepaths[i] + ".h"; 

        WIN32_FILE_ATTRIBUTE_DATA fileData;
        if (!GetFileAttributesExA(filepath.c_str(), GetFileExInfoStandard, &fileData)) {
            assert(false); // TODO: should do a better assertion/debug message -> its when it cant read filedata
        }

        FILETIME recentEditTime = fileData.ftLastWriteTime;
        // If recentEditTime happened after what we have stored
        if(CompareFileTime(&engineData->scriptEditTimes[i], &recentEditTime) == -1) {
            engineData->scriptEditTimes[i] = recentEditTime;
            
            Debug::Log(engineData->scriptFilepaths[i]);

            // Read .h file for relevant changes/additions (to SF_ variables)
            std::vector<std::string> sfLines;
            std::ifstream f(filepath);
            if(!f.is_open()) { assert(false); } // TODO: again, need better assertion/debug msg

            std::string line;
            while(std::getline(f, line)){
                // Remove leading tabs/spaces
                size_t start = line.find_first_not_of(" \t");
                line = (start == std::string::npos) ? "" : line.substr(start);

                // Check if first 3 chars are "SF_"
                if (line.rfind("SF_", 0) == 0) {  // rfind with position 0 checks if 'label' is at the start
                    sfLines.push_back(line);
                    Debug::Log(line);
                }
            } // TODO: this filter is not the most robust, the main issue i see if with multiline comment removing vars (not an issue if ctrl k c is used)

            // Parse data for variables type and name


            // Compare to existing SF_ variables
            // NOTE: there is some nuance here, e.g. if the user reorders variables they should maintain their values


            // Update ALL relevant script data structures and scripts.json
            // TODO: ...
        }
    }
}