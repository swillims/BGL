#include "dataHolder.h"
#include <vector>
#include <iostream>
#include <string>

DataHolder DataHolder::god;

// documentation in the dataholder.h file.

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
    if (physicCap < 0.0001) // if you want more than 10000 frames per second on your engine, no
    {
        currentScene->handle(time);
    }
    else if (physicsTick < 0)
    {
        physicsTick = physicCap;
        currentScene->handle(physicCap); // use physicsCap for time variable to make it deterministic
    }
    //if (frameCap > 0.0001) // if you want more than 10000 frames per second on your engine, no
    //{
    //    return currentScene->render(frameCap);
    //}
    currentScene->render(time, true);

    // trash collection deletion
    trashEmpty();
}

void DataHolder::SetPhysicCap(float cap)
{
    if (cap < .1f)// tolerance check prevents /0. If you want a game that only runs on 10 seconds per frame, you can change this tolerance urself.
    {
        god.physicCap = 0.0f;
    }
    else
    {
        god.physicCap = 1 / cap;
    }
}

void DataHolder::SetFrameCap(float cap)
{
    if (cap < .1f)// tolerance check prevents /0. If you want a game that only runs on 10 seconds per frame, you can change this tolerance urself.
    {
        god.frameCap = 0.0f;
    }
    else
    {
        god.frameCap = 1 / cap;
    }
}

void DataHolder::trashEmpty()
{
    while (!trashList.empty())
    {
        delete trashList.back();
        trashList.pop_back();
    }
}

// If you don't use setters and getters, just go ahead and delete these. 
void DataHolder::setUnCatData(std::string key, std::any data) { uncategorizedData[key] = data; }
bool DataHolder::checkKeyUnCatData(std::string key){return uncategorizedData.contains(key);}
void  DataHolder::deleteUnCatData(std::string key) { uncategorizedData.erase(key); }


