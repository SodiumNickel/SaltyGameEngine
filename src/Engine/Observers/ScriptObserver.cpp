#include "Engine/Observers/Observers.h"

void ScriptObserver::Observe(){
    // TODO: could wire this to only observe when you tab back in to engine...
    for (int i = 0; i < engineData->scriptFilepaths.size(); i++){
        std::string filepath = engineData->scriptFilepaths[i]; // TODO: should be added a bit (path to assetdir) also should just be to header file

        WIN32_FILE_ATTRIBUTE_DATA fileData;
        if (!GetFileAttributesExA(filepath.c_str(), GetFileExInfoStandard, &fileData)) {
            assert(false); // TODO: should do a better assertion/debug message -> its when it cant read filedata
        }

        FILETIME recentEditTime = fileData.ftLastWriteTime;
        // If recentEditTime happened after what we have stored
        if(CompareFileTime(&engineData->scriptEditTimes[i], &recentEditTime) == -1) {
            engineData->scriptEditTimes[i] = recentEditTime;

            // Update ALL relevant script data structures and scripts.json
            // TODO: ...
        }
    }
}