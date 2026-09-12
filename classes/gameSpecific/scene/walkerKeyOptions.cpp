#include "walkerKeyOptions.h"
#include "keyOptions.h"
#include "soundOptions.h"
#include "buttonInput.h"

/*
 * -> IMPORTANT <-
 * The options menus were written while I was still learning and are below standard quality.
 * Do not learn from these.
 * For better examples of how to write UI, go csv writer - https://github.com/swillims/csv-writer
 */

// code may be wrong due to copy pasting from other scene ¯\_(ツ)_/¯
void WalkerKeyOptions::buttonPress(int x)
{
    if (x == uiExit)
    {
        DataHolder::SceneQueue(previous, true);
    }
    else if (x == uiSave)
    {
        saveSetting();
    }
    else if (x == uiW)
    {
        ButtonInput* scene = new ButtonInput(wTitle, this);
        DataHolder::SceneQueue(scene, false);
    }

    else if (x == uiA)
    {
        ButtonInput* scene = new ButtonInput(aTitle, this);
        DataHolder::SceneQueue(scene, false);
    }
    else if (x == uiS)
    {
        ButtonInput* scene = new ButtonInput(sTitle, this);
        DataHolder::SceneQueue(scene, false);
    }
    else if (x == uiD)
    {
        ButtonInput* scene = new ButtonInput(dTitle, this);
        DataHolder::SceneQueue(scene, false);
    }
    else if (x == uiQ)
    {
        ButtonInput* scene = new ButtonInput(qTitle, this);
        DataHolder::SceneQueue(scene, false);
    }
    else if (x == uiE)
    {
        ButtonInput* scene = new ButtonInput(eTitle, this);
        DataHolder::SceneQueue(scene, false);
    }
    else if (x == uiR)
    {
        ButtonInput* scene = new ButtonInput(rTitle, this);
        DataHolder::SceneQueue(scene, false);
    }
    else if (x == uiF)
    {
        ButtonInput* scene = new ButtonInput(fTitle, this);
        DataHolder::SceneQueue(scene, false);
    }
    else if (x == uiEsc)
    {
        ButtonInput* scene = new ButtonInput(escTitle, this);
        DataHolder::SceneQueue(scene, false);
    }
    else if (x==uiGraphicsSettings) // enum name is wrong due to being from old version and not renaming
    {
        KeyOptions* options = new KeyOptions();
        options->previous = previous;
        DataHolder::SceneQueue(options, true);
    }
    else if (x==uiSoundSettings)
    {
        SoundOptions* sound = new SoundOptions();
        sound->previous = previous;
        DataHolder::SceneQueue(sound, true);
    }
}

void WalkerKeyOptions::saveSetting()
{
    {
        StaticAudio::playSoundEffectMulti(bwoo);

        std::string settingsFileName = "metadata/keybind3dsettings";
        std::ostringstream write;

        write << qTitle << ":" << qValue << "\n";
        write << eTitle << ":" << eValue << "\n";
        write << rTitle << ":" << rValue << "\n";
        write << fTitle << ":" << fValue << "\n";
        write << wTitle << ":" << wValue << "\n";
        write << aTitle << ":" << aValue << "\n";
        write << sTitle << ":" << sValue << "\n";
        write << dTitle << ":" << dValue << "\n";
        write << escTitle << ":" << escValue;

        if(util::writeFile(settingsFileName, write.str()))
        {
            std::cout << "keybind settings saved\n";
        }
        else
        {
            std::cout << "fail to write keybind settings\n";
        }
    }
}
