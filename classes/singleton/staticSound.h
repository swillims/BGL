#pragma once

#include "miniaudio.h"

#include <unordered_map>
#include <string>
#include <iostream>
#include <thread>

#include "util.h"
#include "singleton/dataHolder.h"

struct StaticAudio
{
public:

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
    // honestly, consider refactoring into using a struct merging ma_sound* and tags
    inline static std::unordered_map<int, std::vector<int>> tagSoundMap;

    static void init()
    {
        ma_result result = ma_engine_init(NULL, &engine);
        if (result != MA_SUCCESS)
        {
            std::cerr << "Failed to initialize audio engine.\n";
            return;
        }
        std::cout << "Audio engine initialized.\n";
    }
    static void playSoundFromFile(const char* path)
    {
        ma_engine_play_sound(&engine, path, NULL);
    }

    // plays sound effect but does not allow for duplicate sounds
    static void playSoundEffect(int ref)
    {
        ma_sound* sound = soundRefs[ref];
        //ma_sound_stop(sound);
        ma_sound_seek_to_pcm_frame(sound, 0);
        ma_sound_start(sound);
    }
    static void playSoundEffect(const std::string& ref) { playSoundEffect(soundStringRefs[ref]); }

    // sound effect multi is different than regular play sound effect because allows for playing multiple of same sound
    // definition is moved to .cpp file becuase it uses ma_sleep
    static void playSoundEffectMulti(int ref);
    /**
    {
        ma_sound* originalSound = soundRefs[ref];
        ma_sound* clone = new ma_sound;
        ma_sound_init_copy(&engine, originalSound, 0, NULL, clone);
        ma_sound_start(clone);

        // Optionally schedule cleanup:
        std::thread([clone]() {
            while (ma_sound_is_playing(clone)) ma_sleep(10);
            ma_sound_uninit(clone);
            delete clone;
            }).detach();
    }
    //*/

    static void playSoundEffectMulti(const std::string& ref) { playSoundEffectMulti(soundStringRefs[ref]); }

    // This is meant for music and ambience
    static void playSoundLoop(int ref)
    {
        //ma_sound_seek_to_pcm_frame(soundRefs[ref], 0);
        //ma_sound_set_loop_point_in_pcm_frames(soundRefs[ref], 0, ma_sound_get_length_in_pcm_frames(soundRefs[ref]));
        ma_sound_set_looping(soundRefs[ref], true);
        ma_sound_start(soundRefs[ref]);
    }
    static void playSoundLoop(const std::string& ref) { playSoundLoop(soundStringRefs[ref]); }

    // does not work on multi
    static void stopSound(int ref)
    {
        ma_sound_stop(soundRefs[ref]);
    }

    /*static void applyTags(std::string target, const std::vector<std::string>& tags)
    {
        int targetInt = soundStringRefs[target];
        if (!tagSoundMap.contains(targetInt)) // check if it exists
        {
            tagSoundMap[targetInt] = {}; // set to empty if it doesn't exist
        }
        for (const std::string& tag : tags)
        {
            if (!tagStringRefs.contains(tag))
            {
                tagCount++;
                tagStringRefs[tag] = tagCount;
                tagSettings[tagCount] = 1.0f; // "magic number" is 100% default sound setting.
            }
            // this nasty if statement is here to avoid adding same tag twice.
            if (std::find(tagSoundMap[targetInt].begin(), tagSoundMap[targetInt].end(), tagStringRefs[tag]) == tagSoundMap[targetInt].end())
            {
                tagSoundMap[targetInt].push_back(tagStringRefs[tag]);
            }
        }
    }
    //*/
    static void applyTags(int target, const std::vector<std::string>& tags)
    {
        if (!tagSoundMap.contains(target)) // check if it exists
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
            // this nasty if statement is here to avoid adding same tag twice.
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
        if (power)
        {
            // v's value is defaultly squared becuase human hearing is log and not linear
            // pow is used instead of v*v to make it easier to change later. Maybe 2.2 or something idk.
            v = std::pow(v, 2.0f); 
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
            // v's value is defaultly squared becuase human hearing is log and not linear
            // pow is used instead of v*v to make it easier to change later. Maybe 2.2 or something idk.
            v = std::pow(v, 2.0f);
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
        //ma_result result = ma_sound_init_from_file(&engine, path, MA_SOUND_FLAG_DECODE, NULL, NULL, sound);
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

    // contrary to previous functions were it is better to use int, this time use string if you can
    static void unLoad(const std::string& ref);

    static void unLoad(unsigned int ref);
};