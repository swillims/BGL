#include "keyOptions.h"
#include "graphicOptions.h"
#include "walkerKeyOptions.h"
#include "buttonInput.h"

void KeyOptions::buttonPress(int x)
{
    //std::cout<<x<<"\n";
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
    else if (x == uiEsc)
    {
        ButtonInput* scene = new ButtonInput(escTitle, this);
        DataHolder::SceneQueue(scene, false);
    }
    else if (x==uiGraphicsSettings)
    {
        GraphicsOptions* graphics = new GraphicsOptions();
        graphics->previous = previous;
        DataHolder::SceneQueue(graphics, true);
    }
    else if (x==uiSoundSettings)
    {
        WalkerKeyOptions* options = new WalkerKeyOptions();
        options->previous = previous;
        DataHolder::SceneQueue(options, true);
    }
}