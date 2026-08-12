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
    }
    shader3DSimple = StaticDraw::getShader("simple3d");

    if (!StaticDraw::hasShader("projection3d"))
    {
        StaticDraw::compileShader("assets/shaders/projectionview3d.vs", "assets/shaders/simple.fs", "projection3d");
    }
    shader3DProjection = StaticDraw::getShader("projection3d");

    // create ubo
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);
    viewUboRef = StaticDraw::createSharedShaderVariable(view, shader3DProjection, "view").ref;
    projectionUboRef = StaticDraw::createSharedShaderVariable(projection, shader3DProjection, "projection").ref;

    // load vaos
    baseVao = StaticDraw::VAOSimple;

    if (!StaticDraw::hasVAO("3dSimple"))
    {
        StaticDraw::CreateVAO({3, 2}, "3dSimple");
    }
    tileVaoRef = StaticDraw::getVAO("3dSimple").ref;
    tileVaoCount = StaticDraw::getVAO("3dSimple").floatCount;

    // load textures
    if (!StaticDraw::imageFileRefs.contains("tile"))
    {
        StaticDraw::loadImage("assets/gameSpecific/png/frogHop/block.png", "tile");
    }
    tile = StaticDraw::imageFileRefs["tile"];

    if (!StaticDraw::imageFileRefs.contains("greenTile"))
    {
        StaticDraw::loadImage("assets/gameSpecific/png/walk3d/green.png", "greenTile");
    }
    greenTile = StaticDraw::imageFileRefs["greenTile"];

    // load sounds
    StaticAudio::updateSounds();

    // load inputs

    // set physics framerate to 60
    DataHolder::SetPhysicsCap(60);

    aspectChange();
}

float x = 0;

void Walker3D::handle(float time)
{
    processInput(window, time);

    x += time;

    StaticDraw::updateSharedShaderVariable(projectionUboRef, glm::mat4
        (
            1.0f,x,0.0f,0.0f,
            0.0f,1.0f,0.0f,0.0f,
            0.0f,0.0f,1.0f,0.0f,
            0.0f,0.0f,0.0f,1.0f)
            );
}

void Walker3D::render(float time, bool updateDisplay)
{
    // clearing before a draw is correct
    // - clear3D also resets a depth buffer and the depth buffer needs to be set to draw in 3D
    StaticDraw::clear3D();
    batch.clear();

    //StaticDraw::useShader(shader3DSimple);
    StaticDraw::useShader(shader3DProjection);

    StaticDraw::multiDraw
    (
        tile,
        {
            -0.5f, -0.5f, 1.0f,  0.0f, 0.0f,
            0.5f, -0.5f, 0.0f,  1.0f, 0.0f,
            0.5f,  0.5f, 0.0f,  1.0f, 1.0f
        },
        tileVaoRef,
        tileVaoCount
    );

    StaticDraw::multiDraw
    (
        greenTile,
        {
            0.5f, -0.5f, 1.0f,  0.0f, 0.0f,
            -0.5f, -0.5f, 0.0f,  1.0f, 0.0f,
            -0.5f,  0.5f, 0.0f,  1.0f, 1.0f
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
