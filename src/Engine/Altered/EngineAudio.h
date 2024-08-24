#ifndef ENGINEAUDIO_H
#define ENGINEAUDIO_H

#include <deque>
#include <memory>
#include <string>
#include <vector>

#include <soloud.h>
#include <soloud_audiosource.h>


// TODO: should probably do some proxy stuff here, for modifying values
struct Sound {
    std::string filepath;
    bool stream;

    // TODO: maybe they should all be private??? 
    // TODO: should maybe make into class and be private, but i think theres no need
    // as long as user does not edit its fine, gives easier access to Audio class
    int id = -1;
};

// TODO: need access to SoLoud engine somehow, could be here or access from main Game
class EngineAudio {
    private:
        static std::vector<std::unique_ptr<SoLoud::AudioSource>> sounds;
        static std::deque<int> freeIds; // TODO: probably should just use queue or stack, dont have time to think bout rn
    public:
        // Both should only be accessed internally, but have to be static here
        static SoLoud::Soloud soloud; // TODO: do i like these in caps??

        // TODO: definitely need an Audio::ClearLoadedSounds();
        static void Load(Sound& sound);
        static void Deload(Sound& sound);
        static void Play(Sound sound);
};

#endif // ENGINEAUDIO_H