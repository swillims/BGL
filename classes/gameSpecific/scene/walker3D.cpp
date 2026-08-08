#include "walker3D.h"

#include "singleton/staticDraw.h"
#include "singleton/staticInput.h"
#include "singleton/staticSound.h"

void Walker3D::onLoad()
{
    Scene::onLoad();
    StaticDraw::set3DEnabled();

    // have two of a few things to validate loads

    // load shaders
    shaderSimpleRef = StaticDraw::getShader("simple");

    if (!StaticDraw::hasShader("simple3d"))
    {
        StaticDraw::compileShader("assets/shaders/simple3d.vs", "assets/shaders/simple.fs", "simple3d");
        std::cout << "aaa\n";
    }
    shader3DSimple = StaticDraw::getShader("simple3d");

    // load vaos
    baseVao = StaticDraw::VAOSimple;

    if (!StaticDraw::hasVAO("3dSimple"))
    {
        StaticDraw::CreateVAO({3, 2}, "3dSimple");
        std::cout << "bbb\n";
    }
    tileVaoRef = StaticDraw::getVAO("3dSimple").ref;
    tileVaoCount = StaticDraw::getVAO("3dSimple").floatCount;

    if (!StaticDraw::imageFileRefs.contains("tile"))
    {
        StaticDraw::loadImage("assets/gameSpecific/png/frogHop/block.png", "tile");
    }
    tile = StaticDraw::imageFileRefs["tile"];

    // load sounds
    StaticAudio::updateSounds();

    // load inputs






    // set physics framerate to 60
    DataHolder::SetPhysicsCap(60);

    aspectChange();
}

void Walker3D::handle(float time)
{
    processInput(window, time);

}

void Walker3D::render(float time, bool updateDisplay)
{
    // clearing before a draw is correct
    // - clear3D also resets a depth buffer and the depth buffer needs to be set to draw in 3D
    StaticDraw::clear3D();
    batch.clear();

    StaticDraw::useShader(shader3DSimple);


    StaticDraw::multiDraw
    (
        tile,
        {
            -0.5f, -0.5f, 0.0f,  0.0f, 0.0f,
            0.5f, -0.5f, 0.0f,  1.0f, 0.0f,
            0.5f,  0.5f, 1.0f,  1.0f, 1.0f
        },
        tileVaoRef,
        tileVaoCount
    );


    // call super to render
    Scene::render(time, updateDisplay);
}

void Walker3D::processInput(GLFWwindow *window, float time)
{
    StaticInput::Tick();

}

void Walker3D::aspectChange()
{

}

void Walker3D::clean()
{

}
