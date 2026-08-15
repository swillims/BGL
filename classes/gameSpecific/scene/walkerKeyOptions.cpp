#include "walkerKeyOptions.h"
#include "keyOptions.h"
#include "soundOptions.h"
#include "buttonInput.h"

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
    else if (x == uiQ)
    {
        ButtonInput* scene = new ButtonInput(qTitle, this);
        DataHolder::SceneQueue(scene, false);
    }
    else if (x == uiW)
    {
        ButtonInput* scene = new ButtonInput(wTitle, this);
        DataHolder::SceneQueue(scene, false);
    }
    else if (x == uiE)
    {
        ButtonInput* scene = new ButtonInput(eTitle, this);
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