#include "singleton/staticSound.h"

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"


void StaticAudio::playSoundEffectMulti(int ref)
{
    ma_sound* originalSound = soundRefs[ref];
    ma_sound* clone = new ma_sound;
    float audio = ma_sound_get_volume(originalSound);
    ma_sound_init_copy(&engine, originalSound, 0, NULL, clone);
    ma_sound_set_volume(clone, audio);
    ma_sound_start(clone);

    // Optionally schedule cleanup:
    std::thread([clone]() {
        while (ma_sound_is_playing(clone)) ma_sleep(10);
        ma_sound_uninit(clone);
        delete clone;
        }).detach();
}

void StaticAudio::unLoad(const std::string& ref)
{
    if (soundStringRefs.contains(ref))
    {
        int id = soundStringRefs[ref];
        ma_sound_stop(soundRefs[id]);
        ma_sound_uninit(soundRefs[id]);
        delete soundRefs[id];
        soundRefs.erase(id);
        soundStringRefs.erase(ref);
    }
}

void StaticAudio::unLoad(unsigned int ref)
{
    for (const std::pair<const std::string, int>& pair : soundStringRefs)
    {
        if (pair.second == ref)
        {
            unLoad(pair.first);
            return;
        }
    }
    std::cout << "failed to find sound on unload: " << ref << "\n";
}


