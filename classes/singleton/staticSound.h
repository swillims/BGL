#pragma once

#include "miniaudio.h"

#include <unordered_map>
#include <string>
#include <iostream>
#include <thread>

#include "util.h"
#include "singleton/dataHolder.h"

#include <cmath>

struct StaticAudio
{
    inline static ma_engine engine;

    // loaded sounds
    inline static int soundCount;
    inline static std::unordered_map<int, ma_sound*> soundRefs;
    inline static std::unordered_map<std::string, int> soundStringRefs;

    // loaded sound tags
    inline static int tagCount;
    inline static std::unordered_map<int, float> tagSettings;
    inline static std::unordered_map<std::string, int> tagStringRefs;

    // loaded sound tags related to loaded sounds
    inline static std::unordered_map<int, std::vector<int>> tagSoundMap;

    // reference data
    inline static float masterVolume = 1.0; // assumed default value when opening program before loading things

    static void init()
    {
        ma_engine_config config = ma_engine_config_init();

        ma_result result = ma_engine_init(&config, &engine);
        if (result != MA_SUCCESS)
        {
            std::cerr << "Failed to initialize audio engine.\n";
            return;
        }
        std::cout << "Audio engine initialized.\n";\
        ma_device* mm = ma_engine_get_device(&engine);
    }
    static void playSoundFromFile(const char* path)
    {
        ma_engine_play_sound(&engine, path, NULL);
    }

    // plays sound effect but does not allow for duplicate sounds
    static void playSoundEffect(int ref)
    {
        ma_sound* sound = soundRefs[ref];
        ma_sound_seek_to_pcm_frame(sound, 0);
        ma_sound_start(sound);
    }
    static void playSoundEffect(const std::string& ref) { playSoundEffect(soundStringRefs[ref]); }

    // sound effect multi is different from regular play sound effect because allows for playing multiple of same sound
    static void playSoundEffectMulti(int ref);

    static void playSoundEffectMulti(const std::string& ref) { playSoundEffectMulti(soundStringRefs[ref]); }

    // This is meant for music and ambience
    static void playSoundLoop(int ref)
    {
        ma_sound_set_looping(soundRefs[ref], true);
        ma_sound_start(soundRefs[ref]);
    }
    static void playSoundLoop(const std::string& ref) { playSoundLoop(soundStringRefs[ref]); }

    // does not work on multi
    static void stopSound(int ref)
    {
        ma_sound_stop(soundRefs[ref]);
    }

    static void applyTags(int target, const std::vector<std::string>& tags)
    {
        if (!tagSoundMap.contains(target))
        {
            tagSoundMap[target] = {}; // set to empty if it doesn't exist
        }
        for (const std::string& tag : tags)
        {
            if (!tagStringRefs.contains(tag))
            {
                tagCount++;
                tagStringRefs[tag] = tagCount;
                tagSettings[tagCount] = 1.0f; // "magic number" is 100% default sound setting.
            }
            // this ugly "if statement" is here to avoid adding same tag twice.
            if (std::find(tagSoundMap[target].begin(), tagSoundMap[target].end(), tagStringRefs[tag]) == tagSoundMap[target].end())
            {
                tagSoundMap[target].push_back(tagStringRefs[tag]);
            }
        }
    }
    static void applyTags(std::string target, const std::vector<std::string>& tags) { applyTags(soundStringRefs[target], tags); }

    static void setMasterVollume(float v, bool power = true)
    {
        v = std::clamp(v, 0.0f, 1.0f);
        masterVolume = v;
        if (power)
        {
            // power value is defaultly squared because human hearing is log and not linear
            v = std::pow(v, 1.41f);
        }
        ma_engine_set_volume(&engine, v);
    }

    static void updateTagVollume(int tag, float v, bool power = true)
    {
        v = std::clamp(v, 0.0f, 1.0f);
        tagSettings[tag] = v;
    }
    static void updateTagVollume(const std::string& tag, float v, bool power = true) { updateTagVollume(tagStringRefs[tag], v, power); }

    static void updateSoundVollume(int soundRef, bool power = true)
    {
        float v = 1.0f;
        v = std::clamp(v, 0.0f, 1.0f);
        if (tagSoundMap.contains(soundRef))
        {
            for (const int& i : tagSoundMap[soundRef])
            {
                v *= tagSettings[i];
            }
        }
        if (power)
        {
            // power value is defaultly squared because human hearing is log and not linear
            v = std::pow(v, 1.41f);
        }
        ma_sound_set_volume(soundRefs[soundRef], v);
    }

    // this is needed to actually change the vollumes.
    static void updateSounds(bool power = true)
    {
        for (const std::pair<int, ma_sound*>& pair: soundRefs)
        {
            updateSoundVollume(pair.first, power);
        }
    }

    static void load(const char* path, std::string name = "", std::vector<std::string> tags = {}) // code changes name, so don't make it const or a ref
    {
        // Allocate sound on the heap
        ma_sound* sound = new ma_sound;

        ma_result result = ma_sound_init_from_file(&engine, path, 0, NULL, NULL, sound);
        if (result == MA_SUCCESS) {
            std::cout << "Audio Load Success : " << path << "\n";
        }
        else {
            std::cout << "Audio Load Failure : " << path << "\n";
            delete sound;  // free if failed
            return;
        }
        if (name.empty()) { name = util::cleanFileName(path); }

        soundRefs[++soundCount] = sound;
        soundStringRefs[name] = soundCount;
        tagSoundMap[soundCount] = {};
        applyTags(name, tags);
    }

    static void unLoad(const std::string& ref);

    static void unLoad(unsigned int ref);
};