#include "Game/Salty/SaltyAudio.h"

#include <deque>
#include <memory>
#include <vector>

#include <soloud.h>
#include <soloud_audiosource.h>
#include <soloud_wav.h>
#include <soloud_wavstream.h>

// Initializes Soloud engine and Sounds vector
SoLoud::Soloud Audio::soloud;
std::vector<std::unique_ptr<SoLoud::AudioSource>> Audio::sounds;
std::deque<int> Audio::freeIds;

// Give id to Sound object, and when loaded it will assign an id
void Audio::Load(Sound& sound){
    // audioSource has not been loaded yet <-> id = -1
    assert(sound.id == -1); // TODO: maybe this should be just send a debug log later, dont want to crash stuff from randomly loading twice
    int id = -1;

    // To limit amount of space taken
    if(freeIds.empty())
    { 
        // No free ids, expand entity ids
        id = Audio::sounds.size();
        Audio::sounds.resize(id + 1);
    }
    else
    {
        // Free id availible
        id = freeIds.front();
        freeIds.pop_front();
    }

    // Load audio and place into Audio::sounds
    if(sound.stream) { 
        auto wavstream = std::make_unique<SoLoud::WavStream>();
        wavstream->load(("Unique/Assets/" + sound.filepath).c_str()); // TODO: should probably auto add the Unique/Assets/ here
        Audio::sounds[id] = std::move(wavstream);
    }
    else { 
        auto wav = std::make_unique<SoLoud::Wav>();
        wav->load(("Unique/Assets/" + sound.filepath).c_str());
        Audio::sounds[id] = std::move(wav);
    }

    // Signifies that sound has been loaded and can be played
    sound.id = id;
}

// No need to pass this by ref, just need id
void Audio::Play(Sound sound){
    int id = sound.id;
    // 0 <= ensures audioSource has been loaded
    // < size for bounds on loaded sounds
    // != nullptr ensures audioSource has not be deloaded
    assert(0 <= id && id < Audio::sounds.size() && Audio::sounds[id] != nullptr); // TODO: need id to be within len, and then also needs to not be nullptr
    // TODO: tbh, could just load sound here if id = -1... but again, a later change
    // TODO: cont - i think its good to still have a Load() thing incase we want to load at start or specific times, but still a nice failsafe

    SoLoud::AudioSource& aud = *Audio::sounds[id].get();
    Audio::soloud.play(aud);
}