#include "SaltyAudio.h"

#include <memory>
#include <vector>

#include <soloud.h>

// Initializes Soloud engine and Sounds vector
SoLoud::Soloud Audio::Soloud;
std::vector<std::unique_ptr<SoLoud::AudioSource>> Audio::Sounds;

void Audio::Load(AudioSource& audioSource){
    // audioSource has not been loaded yet <-> id = -1
    assert(audioSource.id == -1); 
}

// No need to pass this by ref, just need id
void Audio::Play(AudioSource audioSource){
    assert(true); // TODO: need id to be within len, and then also needs to not be nullptr
}