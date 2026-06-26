#include "dataHolder.h"
#include <vector>
#include <iostream>
#include <string>
#include "scene/scene.h"

DataHolder DataHolder::god;

void DataHolder::init()
{   
    god.physicsTick = 0;
    god.physicCap = 0;
    god.frameTick = 0;
    god.frameCap = 0;
}

void DataHolder::ChangeScene(Scene* s, bool clean)
{
    if (clean) 
    {
        god.currentScene->clean();
        delete god.currentScene; 
    }
    god.currentScene = s;
    s->onLoad();
}

void DataHolder::SceneQueue(Scene* s, bool clean)
{
    god.nextScene = s;
    god.queueDelete = clean;
    god.queue = true;
}

void DataHolder::SceneUpdate()
{
    if (god.queue)
    {
        ChangeScene(god.nextScene, god.queueDelete);
        god.queue = false;
    }
}

void DataHolder::handleScene(float time)
{
    physicsTick -= time;
    frameTick -= time;

    // handle physics
    if (physicCap < 0.0001) // if you want more than 10000 frames per second on your engine, change this number
    {
        currentScene->handle(time);
    }
    else if (physicsTick < 0)
    {
        physicsTick = physicCap;
        currentScene->handle(physicCap); // use physicsCap for time variable to make it deterministic
    }
    if (frameCap < 0.0001) // if you want more than 10000 frames per second on your engine, change this number
    {
        currentScene->render(frameCap);
    }
    else if (frameTick < 0)
    {
        frameTick = frameCap;
        currentScene->render(frameCap);
    }

    // trash collection deletion
    trashEmpty();
}

void DataHolder::setPhysicsCap(float cap)
{
    // tolerance check prevents /0
    if (cap < .01f){ physicCap = 0.0f; }
    else { physicCap = 1 / cap; }
}

void DataHolder::setFrameCap(float cap)
{
    god.frameCapInt = static_cast<int>(cap);
    //tolerance check prevents /0
    if (cap < .1f){ frameCap = 0.0f; }
    else { frameCap = 1 / cap; }
}

void DataHolder::trashEmpty()
{
    while (!trashList.empty())
    {
        trashList.back().deleter(trashList.back().ptr);
        trashList.pop_back();
    }
}
void DataHolder::setUnCatData(std::string key, std::any data) { uncategorizedData[key] = data; }
bool DataHolder::checkKeyUnCatData(std::string key){return uncategorizedData.contains(key);}
void  DataHolder::deleteUnCatData(std::string key) { uncategorizedData.erase(key); }


