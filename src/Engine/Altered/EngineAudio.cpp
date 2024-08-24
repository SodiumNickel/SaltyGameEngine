#include "Engine/Altered/EngineAudio.h"

#include <deque>
#include <memory>
#include <vector>

#include <soloud.h>
#include <soloud_audiosource.h>
#include <soloud_wav.h>
#include <soloud_wavstream.h>

// TODO: i kinda wanna move this into EngineAssetManager

// Initializes Soloud engine and Sounds vector
SoLoud::Soloud EngineAudio::soloud;
std::vector<std::unique_ptr<SoLoud::AudioSource>> EngineAudio::sounds;
std::deque<int> EngineAudio::freeIds;

// Give id to Sound object, and when loaded it will assign an id
void EngineAudio::Load(Sound& sound){
    // audioSource has not been loaded yet <-> id = -1
    // TODO: agreed with the below, log an error (i.e. color it red)
    assert(sound.id == -1); // TODO: maybe this should be just send a debug log later, dont want to crash stuff from randomly loading twice
    int id = -1;

    // To limit amount of space taken
    if(freeIds.empty())
    { 
        // No free ids, expand entity ids
        id = EngineAudio::sounds.size();
        EngineAudio::sounds.resize(id + 1);
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
        EngineAudio::sounds[id] = std::move(wavstream);
    }
    else { 
        auto wav = std::make_unique<SoLoud::Wav>();
        wav->load(("Unique/Assets/" + sound.filepath).c_str());
        EngineAudio::sounds[id] = std::move(wav);
    }

    // Signifies that sound has been loaded and can be played
    sound.id = id;
}

// NOTE: this will also stop the sound
void EngineAudio::Deload(Sound& sound){
    // assert(sound.id != -1);
    // TODO: allowing multiple Deload calls on one sound right now
    // Will not do anything on further calls

    if(sound.id != -1){
        EngineAudio::soloud.stopAudioSource(*EngineAudio::sounds[sound.id].get());
        EngineAudio::sounds[sound.id].reset();
        sound.id = -1;
    }
}


// No need to pass this by ref, just need id
void EngineAudio::Play(Sound sound){
    int id = sound.id;
    // 0 <= ensures audioSource has been loaded
    // < size for bounds on loaded sounds
    // != nullptr ensures audioSource has not be deloaded
    assert(0 <= id && id < EngineAudio::sounds.size() && EngineAudio::sounds[id] != nullptr); // TODO: need id to be within len, and then also needs to not be nullptr
    // TODO: tbh, could just load sound here if id = -1... but again, a later change
    // TODO: cont - i think its good to still have a Load() thing incase we want to load at start or specific times, but still a nice failsafe

    SoLoud::AudioSource& aud = *EngineAudio::sounds[id].get();
    EngineAudio::soloud.play(aud);
}