
#include "graphicOptions.h"

#include "soundOptions.h"


void GraphicsOptions::buttonPress(int x)
{
    //std::cout<<x<<"\n";
    if (x == 0)
    {
        DataHolder::SceneQueue(previous, true);
    }
    else if (x == uiScaleUp)
    {
        resScale++;
        scaleValue = resScale;
    }
    else if (x == uiScaleDown)
    {
        if (resScale>1){resScale--;}
        scaleValue = resScale;
    }
    else if (x == uiX1)
    {
        StaticDraw::resize(StaticDraw::w+1000, StaticDraw::h);
    }
    else if (x == uiX2)
    {
        StaticDraw::resize(StaticDraw::w+100, StaticDraw::h);
    }
    else if (x == uiX3)
    {
        StaticDraw::resize(StaticDraw::w+10, StaticDraw::h);
    }
    else if (x == uiX4)
    {
        StaticDraw::resize(StaticDraw::w+1, StaticDraw::h);
    }
    else if (x == uiY1)
    {
        StaticDraw::resize(StaticDraw::w, StaticDraw::h+1000);
    }
    else if (x == uiY2)
    {
        StaticDraw::resize(StaticDraw::w, StaticDraw::h+100);
    }
    else if (x == uiY3)
    {
        StaticDraw::resize(StaticDraw::w, StaticDraw::h+10);
    }
    else if (x == uiY4)
    {
        StaticDraw::resize(StaticDraw::w, StaticDraw::h+1);
    }
    else if (x == uiX1d && StaticDraw::w>=1100)
    {
        StaticDraw::resize(StaticDraw::w-1000, StaticDraw::h);
    }
    else if (x == uiX2d && StaticDraw::w>=200)
    {
        StaticDraw::resize(StaticDraw::w-100, StaticDraw::h);
    }
    else if (x == uiX3d && StaticDraw::w>=110)
    {
        StaticDraw::resize(StaticDraw::w-10, StaticDraw::h);
    }
    else if (x == uiX4d && StaticDraw::w>=101)
    {
        StaticDraw::resize(StaticDraw::w-1, StaticDraw::h);
    }
    else if (x == uiY1d && StaticDraw::h>=1100)
    {
        StaticDraw::resize(StaticDraw::w, StaticDraw::h-1000);
    }
    else if (x == uiY2d && StaticDraw::h>=200)
    {
        StaticDraw::resize(StaticDraw::w, StaticDraw::h-100);
    }
    else if (x == uiY3d && StaticDraw::h>=110)
    {
        StaticDraw::resize(StaticDraw::w, StaticDraw::h-10);
    }
    else if (x == uiY4d && StaticDraw::h>=101)
    {
        StaticDraw::resize(StaticDraw::w, StaticDraw::h-1);
    }
    else if (x==uiSoundSettings)
    {
        SoundOptions* sound = new SoundOptions();
        sound->previous = previous;
        DataHolder::SceneQueue(sound, true);
    }
}