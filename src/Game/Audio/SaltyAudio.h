#ifndef SALTYAUDIO_H
#define SALTYAUDIO_H
// Will be included in user scripts, indicated by "Salty" being part of name

#include <memory>
#include <string>
#include <vector>

#include <soloud.h>

// TODO: not sure i want the name to be AudioSource... will  think about this
struct AudioSource {
    std::string filepath;
    bool stream;

    // TODO: maybe they should all be private??? 
    // TODO: should maybe make into class and be private, but i think theres no need
    // as long as user does not edit its fine, gives easier access to Audio class
    int id;
};

// TODO: need access to SoLoud engine somehow, could be here or access from main Game
class Audio {
    // TODO: need to consider if multiple things reference the same audio source
    // going to give id to AudioSource object, and when loaded it will assign an id
    public:
        static void Load(AudioSource audioSource);
    private:
        std::vector<std::unique_ptr<SoLoud::AudioSource>> sounds;
};

#endif // SALTYAUDIO_H